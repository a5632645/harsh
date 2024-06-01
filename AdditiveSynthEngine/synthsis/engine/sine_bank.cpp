#include "SineBank.h"

#include "utli/convert.h"

namespace mana {
SineBank::SineBank() {
    phase_table_.resize(kNumPartials);
    last_volume_table_.resize(kNumPartials);
    current_volume_table_.resize(kNumPartials);
    freq_table_.resize(kNumPartials);
}

void mana::SineBank::Init(float sample_rate, float update_rate, int update_skip) {
    sample_rate_ = sample_rate;
    one_div_nyquist_rate = 2.0f / sample_rate;
    nyquist_rate_ = sample_rate / 2.0f;
    update_rate_ = update_rate;

    // smooth time should smaller than inv update_rate
    cos_table_.resize(update_skip);
    for (int i = 0; i < update_skip; ++i) {
        auto nor = static_cast<float>(i) / static_cast<float>(update_skip); // 0..1
        auto pi = nor * std::numbers::pi_v<float>;                          // 0..pi
        cos_table_[i] = 0.5f - 0.5f * std::cos(pi);                         // cos interpole
    }
}

void SineBank::LoadPartials(Partials & partials) {
    size_t num_processed = count_process_particles(partials);
    processed_partials_ = std::max(processed_partials_, num_processed);
    active_partials_ = std::min(processed_partials_, max_active_partials_);
    sr_pos_ = 0;

    if (partials.update_phase) {
        std::ranges::copy(partials.phases, phase_table_.begin());
        partials.update_phase = false;
    }

    for (size_t i = 0; i < kNumPartials; ++i) {
        const auto normalized_frequency = partials.freqs[i];
        const float radix_frequency = normalized_frequency * std::numbers::pi_v<float>;
        freq_table_[i] = std::polar(1.0f, radix_frequency);

        //if (normalized_frequency <= 0.0f || normalized_frequency > 1.0f) {
        //    target_volume_table_[i] = 0.0f;
        //}
        //else {
        //    target_volume_table_[i] = partials.gains[i];
        //}
    }

    std::ranges::copy(current_volume_table_, last_volume_table_.begin());
    std::ranges::copy(partials.gains, current_volume_table_.begin());

    #ifdef AD_ENABLE_SIMD
    // round num_partials to simd_size
    num_phase_loop_ = std::ceil(processed_partials_ / static_cast<float>(simd_size)) * simd_size;
    num_volume_loop_ = std::ceil(active_partials_ / static_cast<float>(simd_size)) * simd_size;
    #endif
}

float SineBank::SrTick() {
    #ifdef AD_ENABLE_SIMD
    for (size_t i = 0; i < num_phase_loop_; i += simd_size) {
        auto vec_target_vol = xsimd::load_aligned(target_volume_table_.data() + i);
        auto vec_old_vol = xsimd::load_aligned(volume_table_.data() + i);
        auto vec_vol = amp_smooth_factor_ * vec_old_vol + amp_smooth_factor2_ * vec_target_vol;
        xsimd::store_aligned(volume_table_.data() + i, vec_vol);
    }

    for (size_t i = 0; i < num_phase_loop_; i += simd_size) {
        auto vec_p = xsimd::load_aligned(phase_table_.data() + i);
        auto vec_pinc = xsimd::load_aligned(freq_table_.data() + i);
        auto vec_new_p = xsimd::mul(vec_p, vec_pinc);
        vec_new_p.store_aligned(phase_table_.data() + i);
    }

    float output{};
    for (size_t i = 0; i < num_volume_loop_; ++i) {
        output += phase_table_[i].imag() * volume_table_[i];
    }
    return output;
    #else
    for (size_t i = 0; i < processed_partials_; ++i) {
        phase_table_[i] *= freq_table_[i];
    }

    float output{};
    float current_cos_lerp = cos_table_[sr_pos_++];
    for (size_t i = 0; i < active_partials_; ++i) {
        auto last_gain = last_volume_table_[i];
        auto curr_gain = current_volume_table_[i];
        auto gain = last_gain + (curr_gain - last_gain) * current_cos_lerp;

        output += phase_table_[i].imag() * gain;
        //output += phase_table_[i].imag() * target_volume_table_[i];
    }
    return output;
    #endif
}
}