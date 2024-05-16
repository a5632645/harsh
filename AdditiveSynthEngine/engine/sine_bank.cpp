#include "SineBank.h"

namespace mana {
SineBank::SineBank() {
    phase_table_.resize(kNumPartials);
    volume_table_.resize(kNumPartials);
    target_volume_table_.resize(kNumPartials);
    freq_table_.resize(kNumPartials);
}

void mana::SineBank::Init(float sample_rate) {
    sample_rate_ = sample_rate;
    one_div_nyquist_rate = 2.0f / sample_rate;
    nyquist_rate_ = sample_rate / 2.0f;
}

void SineBank::SetSmoothTime(float milliseconds) {
    amp_smooth_factor_ = std::exp(-1.0F / (milliseconds * sample_rate_ / 1000.0F));
    amp_smooth_factor2_ = 1.0f - amp_smooth_factor_;
}

void SineBank::LoadPartials(Partials & partials) {
    size_t num_processed = count_process_particles(partials);
    processed_partials_ = std::max(processed_partials_, num_processed);
    active_partials_ = std::min(processed_partials_, max_active_partials_);

    if (partials.update_phase) {
        std::ranges::copy(partials.phases, phase_table_.begin());
        partials.update_phase = false;
    }

    for (size_t i = 0; i < kNumPartials; ++i) {
        const auto normalized_frequency = partials.freqs[i];
        const float radix_frequency = normalized_frequency * std::numbers::pi_v<float>;
        freq_table_[i] = std::polar(1.0f, radix_frequency);

        if (normalized_frequency <= 0.0f || normalized_frequency > 1.0f) {
            target_volume_table_[i] = 0.0f;
        }
        else {
            target_volume_table_[i] = partials.gains[i];
        }
    }

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
        volume_table_[i] = volume_table_[i] * amp_smooth_factor_
            + target_volume_table_[i] * amp_smooth_factor2_;
}

    for (size_t i = 0; i < processed_partials_; ++i) {
        phase_table_[i] *= freq_table_[i];
    }

    float output{};
    for (size_t i = 0; i < active_partials_; ++i) {
        output += phase_table_[i].imag() * volume_table_[i];
    }
    return output;
    #endif
}
}