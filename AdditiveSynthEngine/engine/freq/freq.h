#pragma once

#include "engine/IProcessor.h"
#include "param/voice_param.h"

namespace mana {

class FreqProcessor : public IProcessor {
public:
    FreqProcessor(const param::VoiceParam& param);

    // 通过 IProcessor 继承
    void Init(float sample_rate) override;

    void OnUpdateTick(int skip) override;

    void OnNoteOn(int note) override;

    void OnNoteOff() override;

    void Process(Partials& partials) override;
private:
    const param::VoiceParam& voice_param_;

    float reciprocal_nyquist_rate_{};
    float note_pitch_{};
    float base_frequency_{};
};

}