#pragma once

#include <array>
#include <cstdlib>
#include "Oscillor.h"
#include "modulation/ParamBank.h"
#include "resynthsis/resynthsis_data.h"
#include "utli/spin_lock.h"
#include "engine/modulation/curve.h"

namespace mana {
struct SimplePixel {
    int r;
    int g;
    int b;
};

class Synth {
public:
    Synth();

    void NoteOn(int note, float velocity);

    void NoteOff(int note, float velocity);

    void Render(size_t numFrame);

    void Init(size_t bufferSize, float sampleRate, float update_rate);

    void update_state(int step);

    decltype(auto) getBuffer() const {
        return (audio_buffer_);
    }

    const Oscillor& GetDisplayOscillor() const;
    ParamBank& GetParamBank() { return synth_params_.GetParamBank(); }
    CurveManager& GetCurveManager() { return synth_params_.GetCurveManager(); }
    SynthParams& GetSynthParams() { return synth_params_; }
    ResynthsisFrames& GetResynthsisFrames() { return resynthsis_frames_; }

    void SetResynthsisFrames(ResynthsisFrames new_frame);
    bool IsResynthsisAvailable() const { return !resynthsis_frames_.frames.empty(); }
    ResynthsisFrames CreateResynthsisFramesFromAudio(const std::vector<float>& audio_in, float sample_rate);
    ResynthsisFrames CreateResynthsisFramesFromImage(const std::vector<std::vector<SimplePixel>>& audio_in);
    std::vector<std::string_view> GetModulatorIds() const { return m_oscillators.front().GetModulatorIds(); }
    std::vector<std::string_view> GetModulableParamIds() const { return m_oscillators.front().GetModulableParamIds(); }
    utli::SpinLock& GetSynthLock() { return synth_lock_; }

    std::pair<bool, ModulationConfig*> CreateModulation(std::string_view modulator, std::string_view param);
    void RemoveModulation(ModulationConfig& config);
private:
    utli::SpinLock synth_lock_;
    ResynthsisFrames resynthsis_frames_;
    SynthParams synth_params_;
    std::vector<float> audio_buffer_;
    std::vector<Oscillor> m_oscillators;
    std::vector<std::shared_ptr<ModulationConfig>> modulation_configs_;
    size_t m_rrPosition{};

    float sample_rate_{};
    float update_rate_{};
    int update_skip_{};
    int update_counter_{};
    FloatParameter* output_gain_{};
};
}