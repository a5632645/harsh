#pragma once

#include <array>
#include <cstdlib>
#include "Oscillor.h"
#include "modulation/ParamBank.h"
#include "param/synth_param.h"
#include "engine/resynthsis_data.h"

namespace mana {
class Synth {
public:
    Synth();

    void NoteOn(int note, float velocity);

    void NoteOff(int note, float velocity);

    void Render(size_t numFrame);

    void Init(size_t bufferSize, float sampleRate);

    void update_state(int step);

    decltype(auto) getBuffer() const {
        return (audio_buffer_);
    }

    const Oscillor& GetDisplayOscillor() const;
    const ParamBank& GetParamBank() const { return param_bank_; }
    SynthParam& GetSynthParam() { return synth_param_; }
    ResynthsisFrames& GetResynthsisFrames() { return timber_resynthsis_frames_; }
    void CreateResynthsisFrames(const std::vector<float>& audio_in);
private:
    void BindParam();

    ParamBank param_bank_;
    SynthParam synth_param_;
    ResynthsisFrames timber_resynthsis_frames_;
    std::vector<float> audio_buffer_;
    std::vector<Oscillor> m_oscillators;
    size_t m_rrPosition{};
    float sample_rate_{};
};
}