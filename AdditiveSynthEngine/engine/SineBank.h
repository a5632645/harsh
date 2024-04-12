#include <array>
#include <cmath>
#include <numbers>
#include <complex>
#include "partials.h"
#include "engine/EngineConfig.h"
#include "utli/DebugMarco.h"

namespace mana {
class SineBank {
public:
    void Init(float sample_rate) {
        DSP_INIT;

        sample_rate_ = sample_rate;
        m_one_div_nyquist_rate = 2.0f / sample_rate;
        m_nyquist_rate = sample_rate / 2.0f;
    }

    void set_smooth_time(float milliseconds) {
        DSP_INIT_ASSERT;

        m_a = std::exp(-1.0F / (milliseconds * sample_rate_ / 1000.0F));
    }

    void load_particles(Partials& particles) {
        // find particles
        const size_t num_processed = count_process_particles(particles);
        m_processed_particles = std::max(m_processed_particles, num_processed);
        m_active_particles = std::min(m_processed_particles, m_max_particles);

        if (particles.update_phase) {
            load_phase_table(particles);
            particles.update_phase = false;
        }

        for (size_t i = 0; i < kNumPartials; ++i) {
            const auto normalized_frequency = particles.freqs[i];
            const float radix_frequency = normalized_frequency * std::numbers::pi_v<float>;
            m_freq_table[i] = std::polar(1.0f, radix_frequency);

            if (normalized_frequency <= 0.0f || normalized_frequency > 1.0f) {
                m_target_volume_table[i] = 0.0f;
            }
            else {
                m_target_volume_table[i] = particles.gains[i];
            }
        }
    }

    float process_once() {
        for (size_t i = 0; i < m_processed_particles; ++i) {
            m_volume_table[i] = m_a * m_volume_table[i] + (1.0F - m_a) * m_target_volume_table[i];
        }

        for (size_t i = 0; i < m_processed_particles; ++i) {
            m_current_table[i] *= m_freq_table[i];
        }

        float output{};
        for (size_t i = 0; i < m_active_particles; ++i) {
            output += m_current_table[i].imag() * m_volume_table[i];
        }
        return output;
    }

    void note_on() {
        m_processed_particles = 0;
        m_active_particles = 0;
    }

    void set_max_particles(size_t number) {
        m_max_particles = number;
    }
private:
    void load_phase_table(const Partials& particles) {
        static constexpr auto two_pi = std::numbers::pi_v<float> *2.0f;
        for (size_t i = 0; i < kNumPartials; ++i) {
            m_current_table[i] = std::polar(1.0f, particles.phases[i] * two_pi);
        }
    }

    size_t count_process_particles(const Partials& particles) {
        for (size_t i = kNumPartials - 1; i > 0; --i) {
            if (particles.freqs[i] <= 0.0f || particles.freqs[i] > 1.0f) {
                return i;
            }
        }
        return kNumPartials;
    }

    DSP_INIT_DEFINE;

    std::array<float, kNumPartials> m_target_volume_table{};
    std::array<float, kNumPartials> m_volume_table{};
    std::array<std::complex<float>, kNumPartials> m_freq_table{};
    std::array<std::complex<float>, kNumPartials> m_current_table{};
    size_t m_max_particles{};
    size_t m_processed_particles{};
    size_t m_active_particles{};
    float m_a{};
    float sample_rate_{};
    float m_one_div_nyquist_rate{};
    float m_nyquist_rate{};
};
}