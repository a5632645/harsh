#include "SineBank.h"

#include "utli/convert.h"

namespace mana {
SineBank::SineBank() {
    phase_table_.resize(kNumPartials);
    current_volume_table_.resize(kNumPartials);
    freq_table_.resize(kNumPartials);
    sinc_last1_gain_.resize(kNumPartials);
    sinc_last2_gain_.resize(kNumPartials);
}

void mana::SineBank::Init(float sample_rate, float update_rate, int update_skip) {
    sample_rate_ = sample_rate;
    inv_sample_rate_ = 1.0f / sample_rate;
    update_rate_ = update_rate;
    update_skip_ = update_skip;

    constexpr auto pi = std::numbers::pi_v<float>;
    //constexpr auto hamming_main_lobe_width_bin = 2.736f;
    constexpr auto hamming_main_lobe_width_bin = 3.0f;
    auto polyphase_fir_length = 2;
    auto fir_order = polyphase_fir_length * update_skip;
    auto fir_length = fir_order + 1;
    auto passband_freqlen = hamming_main_lobe_width_bin / fir_length;
    auto fir_center = fir_order * 0.5f;
    std::vector<float> fir_org_lut(fir_length);
    auto fir_cut_f = pi * (update_rate * 0.5f / sample_rate + passband_freqlen * 0.5f);

    auto lowpass_sinc = [=](int n) {
        auto x = n - fir_center;
        if (std::abs(x) <= 1e-7) {
            return fir_cut_f / pi;
        }
        return std::sin(fir_cut_f * x) / (pi * x);
    };
    auto hamming_window = [=](int n) {
        return 0.53836f - 0.46164f * std::cos(pi * 2.0f * n / fir_order);
    };

    auto gain_up = update_skip_ * polyphase_fir_length;
    for (int i = 0; i < fir_length; ++i) {
        fir_org_lut.push_back(lowpass_sinc(i) * hamming_window(i));
    }

    // none fir polyphase into zero keep polyphase
    std::ranges::reverse(fir_org_lut);
    for (int i = 0; i < update_skip; ++i) {
        {
            float temp{};
            for (int j = 0; j < i + 1; ++j) {
                temp += fir_org_lut[j];
            }
            fir_curr_lut_.push_back(temp);
        }
        {
            float temp{};
            for (int j = i + 1; j < update_skip + i + 1; ++j) {
                temp += fir_org_lut[j];
            }
            fir_last1_lut_.push_back(temp);
        }
        {
            float temp{};
            for (int j = update_skip + i + 1; j < 2 * update_skip + 1; ++j) {
                temp += fir_org_lut[j];
            }
            fir_last2_lut_.push_back(temp);
        }
    }
}

void SineBank::LoadPartials(Partials& partials) {
    size_t num_processed = count_process_particles(partials);
    processed_partials_ = std::max(processed_partials_, num_processed);
    active_partials_ = std::min(processed_partials_, max_active_partials_);
    sr_pos_ = 0;

    if (partials.update_phase) {
        std::ranges::copy(partials.phases, phase_table_.begin());
        partials.update_phase = false;
    }

    std::ranges::copy(sinc_last1_gain_, sinc_last2_gain_.begin());
    std::ranges::copy(current_volume_table_, sinc_last1_gain_.begin());

    for (size_t i = 0; i < kNumPartials; ++i) {
        const auto normalized_frequency = partials.freqs[i] * inv_sample_rate_;
        const float radix_frequency = normalized_frequency * std::numbers::pi_v<float>;
        freq_table_[i] = std::polar(1.0f, radix_frequency);

        if (normalized_frequency < 0.0f || normalized_frequency > Partials::kMaxFreq) {
            current_volume_table_[i] = 0.0f;
        }
        else {
            current_volume_table_[i] = partials.gains[i];
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
        auto vec_p = xsimd::load_aligned(phase_table_.data() + i);
        auto vec_pinc = xsimd::load_aligned(freq_table_.data() + i);
        auto vec_new_p = xsimd::mul(vec_p, vec_pinc);
        vec_new_p.store_aligned(phase_table_.data() + i);
    }

    float output{};
    auto fir_coef0 = fir_curr_lut_[sr_pos_];
    auto fir_coef1 = fir_last1_lut_[sr_pos_];
    auto fir_coef2 = fir_last2_lut_[sr_pos_];
    for (size_t i = 0; i < num_volume_loop_; i += simd_size) {
        auto vec_curr = xsimd::load_aligned(current_volume_table_.data() + i);
        auto vec_last1 = xsimd::load_aligned(sinc_last1_gain_.data() + i);
        auto vec_last2 = xsimd::load_aligned(sinc_last2_gain_.data() + i);
        auto gain = vec_curr * fir_coef0
            + vec_last1 * fir_coef1
            + vec_last2 * fir_coef2;
        auto vec_phase = xsimd::load_aligned(phase_table_.data() + i);
        auto f = vec_phase.imag() * gain;
        output += xsimd::reduce_add(f);
    }
    ++sr_pos_;
    return output;
    #else
    for (size_t i = 0; i < processed_partials_; ++i) {
        phase_table_[i] *= freq_table_[i];
}

    float output{};
    auto fir_coef0 = fir_curr_lut_[sr_pos_];
    auto fir_coef1 = fir_last1_lut_[sr_pos_];
    auto fir_coef2 = fir_last2_lut_[sr_pos_];
    for (size_t i = 0; i < active_partials_; ++i) {
        auto gain = current_volume_table_[i] * fir_coef0
            + sinc_last1_gain_[i] * fir_coef1
            + sinc_last2_gain_[i] * fir_coef2;
        output += phase_table_[i].imag() * gain;
    }
    ++sr_pos_;
    return output;
    #endif
}
}