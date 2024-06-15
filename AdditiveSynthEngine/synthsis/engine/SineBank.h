#pragma once

#include <vector>
#include <array>
#include <cmath>
#include <numbers>
#include <complex>
#include "partials.h"
#include "engine/EngineConfig.h"
#include <xsimd/xsimd.hpp>

namespace mana {
class SineBank {
public:
    using batch_float = xsimd::batch<float>;
    using batch_complex = xsimd::batch<std::complex<float>>;
    using batch_float_vector = std::vector<float, xsimd::aligned_allocator<float, sizeof(batch_float)>>;
    using batch_complex_vector = std::vector<std::complex<float>, xsimd::aligned_allocator<std::complex<float>, sizeof(batch_complex)>>;
    static constexpr auto simd_size = batch_float::size;

    SineBank();

    void Init(float sample_rate, float update_rate, int update_skip);

    void LoadPartials(Partials& partials);

    float SrTick();

    void ResetState() {
        processed_partials_ = 0;
        active_partials_ = 0;
    }

    void SetNumMaxActivePartials(size_t number) {
        max_active_partials_ = number;
    }

    decltype(auto) GetPhaseTable() const {
        return (phase_table_);
    }

    decltype(auto) GetSmoothGainTable() const { return (current_volume_table_); }
private:
    size_t count_process_particles(const Partials& particles) {
        // find last not aliasing partial idx
        for (size_t i = kNumPartials - 1; i > 0; --i) {
            if (particles.freqs[i] >= 0.0f && particles.freqs[i] <= 1.0f) {
                return i + 1;
            }
        }
        return 0;
    }

    // smoothing
    int sr_pos_{};
    //int lut_pos3_{};
    //int lut_pos2_{};
    //int lut_pos1_{};
    // std::vector<float> cos_table_;
    // batch_float_vector sinc_last3_gain_;
    batch_float_vector sinc_last2_gain_;
    batch_float_vector sinc_last1_gain_;
    batch_float_vector current_volume_table_{};
    // batch_float_vector fir_lut_;
    batch_float_vector fir_curr_lut_;
    batch_float_vector fir_last1_lut_;
    batch_float_vector fir_last2_lut_;

    // batch_float_vector last_volume_table_{};
    batch_complex_vector freq_table_{};
    batch_complex_vector phase_table_{};

    size_t max_active_partials_{};
    size_t num_phase_loop_{};
    size_t processed_partials_{};
    size_t active_partials_{};
    size_t num_volume_loop_{};
    float sample_rate_{};
    float one_div_nyquist_rate{};
    float update_rate_{};
    int update_skip_{};
    float nyquist_rate_{};
};
}