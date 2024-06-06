#pragma once

#include <array>
#include <cstdlib>
#include "Oscillor.h"
#include "modulation/ParamBank.h"
#include "resynthsis/resynthsis_data.h"
#include "resynthsis/image_base.h"
#include <mutex>

namespace mana {
class Synth {
public:
    Synth(std::shared_ptr<ParamCreator> creator, size_t num_osc);

    void NoteOn(int note, float velocity);

    void NoteOff(int note, float velocity);

    void Render(size_t numFrame);
    void Render(float* buffer, int num_frame);

    void Init(size_t bufferSize, float sampleRate, float update_rate);

    void update_state(int step);
    int GetUpdateSkip() const { return update_skip_; }

    decltype(auto) getBuffer() const {
        return (audio_buffer_);
    }

    const Oscillor& GetDisplayOscillor() const;
    const Oscillor& GetOscillor(int idx) const { return m_oscillators[idx]; }
    ParamBank& GetParamBank() { return synth_params_.GetParamBank(); }
    CurveBank& GetCurveManager() { return synth_params_.GetCurveBank(); }
    SynthParams& GetSynthParams() { return synth_params_; }
    ResynthsisFrames& GetResynthsisFrames() { return resynthsis_frames_; }

    void SetResynthsisFrames(ResynthsisFrames new_frame);
    bool IsResynthsisAvailable() const { return !resynthsis_frames_.frames.empty(); }
    ResynthsisFrames CreateResynthsisFramesFromAudio(const std::vector<float>& audio_in, float sample_rate) const;
    ResynthsisFrames CreateResynthsisFramesFromImage(std::unique_ptr<ImageBase> image_in);
    std::vector<std::string_view> GetModulatorIds() const { return m_oscillators.front().GetModulatorIds(); }
    std::vector<std::string_view> GetModulableParamIds() const { return m_oscillators.front().GetModulableParamIds(); }
    decltype(auto) GetSynthLock() { return (synth_lock_); }

    std::pair<bool, ModulationConfig*> CreateModulation(std::string_view modulator, std::string_view param);
    void RemoveModulation(ModulationConfig& config);
    int GetModulatorCount() const { return synth_params_.GetModulationCount(); }
    std::shared_ptr<ModulationConfig> GetModulationConfig(int index) { return synth_params_.GetModulation(index); }
private:
    std::mutex synth_lock_;
    ResynthsisFrames resynthsis_frames_;
    SynthParams synth_params_;
    std::vector<float> audio_buffer_;
    std::vector<Oscillor> m_oscillators;
    size_t m_rrPosition{};
    const size_t num_oscillor_{};

    float sample_rate_{};
    float update_rate_{};
    int update_skip_{};
    int update_counter_{};
    FloatParameter* output_gain_{};
};
}