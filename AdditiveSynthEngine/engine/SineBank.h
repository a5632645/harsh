#include <array>
#include <cmath>
#include <numbers>
#include <complex>
#include "partials.h"
#include "engine/EngineConfig.h"

namespace mana {
class SineBank {
public:
    void Init(float sample_rate) {
        sample_rate_ = sample_rate;
        one_div_nyquist_rate = 2.0f / sample_rate;
        nyquist_rate_ = sample_rate / 2.0f;
    }

    void SetSmoothTime(float milliseconds) {
        amp_smooth_factor_ = std::exp(-1.0F / (milliseconds * sample_rate_ / 1000.0F));
    }

    void LoadPartials(Partials& partials) {
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
    }

    float SrTick() {
        for (size_t i = 0; i < processed_partials_; ++i) {
            volume_table_[i] = amp_smooth_factor_ * volume_table_[i] + (1.0F - amp_smooth_factor_) * target_volume_table_[i];
        }

        for (size_t i = 0; i < processed_partials_; ++i) {
            phase_table_[i] *= freq_table_[i];
        }

        float output{};
        for (size_t i = 0; i < active_partials_; ++i) {
            output += phase_table_[i].imag() * volume_table_[i];
        }
        
        return output;
    }

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

    std::array<float, kNumPartials> target_volume_table_{};
    std::array<float, kNumPartials> volume_table_{};
    std::array<std::complex<float>, kNumPartials> freq_table_{};
    std::array<std::complex<float>, kNumPartials> phase_table_{};
    size_t max_active_partials_{};
    size_t processed_partials_{};
    size_t active_partials_{};
    float amp_smooth_factor_{};
    float sample_rate_{};
    float one_div_nyquist_rate{};
    float nyquist_rate_{};
};
}