#include "resynthsis.h"

#include "engine/synth.h"
#include "param/timber.h"
#include "param/param.h"

namespace mana {
void Resynthesis::Process(Partials& partials) {
    constexpr auto kFFtSize = kNumPartials * 2;
    constexpr auto kFFtHop = kFFtSize / 4;
    constexpr auto kHarmonics = kNumPartials;

    const auto& frames = synth_.GetResynthsisFrames();
    auto frame_idx = static_cast<size_t>((frames.frames.size() - 1) * frame_pos_);
    const auto& frame = frames.frames.at(frame_idx);
    std::ranges::copy(frame.gains, partials.gains.begin());

    if (freq_scale_ == float{}) return;

    auto freq_scale = partials.base_frequency / frames.data_series_freq * freq_scale_;
    for (int i = 0; i < kNumPartials; ++i) {
        partials.freqs[i] += freq_scale * frame.freq_diffs[i];
        //auto ratio = partials.freqs[i] / partials.base_frequency;
        //partials.pitches[i] = partials.base_pitch + 12.0f * std::log2(ratio);
    }
}

void Resynthesis::OnUpdateTick(const SynthParam& params, int skip, int module_idx) {
    formant_mix_ = param::FloatParam<param::Resynthsis_FormantMix>::GetNumber(params.timber.args);
    formant_shift_ = param::FloatParam<param::Resynthsis_FormantShift>::GetNumber(params.timber.args);
    freq_scale_ = param::FloatParam<param::Resynthsis_FreqScale>::GetNumber(params.timber.args);
    //frame_pos_ = param::FloatParam<param::Resynthsis_FramePos>::GetNumber(params.timber.args);

    if (synth_.GetResynthsisFrames().frames.empty()) return;

    auto pos_inc = (float)skip / kFFtHop / synth_.GetResynthsisFrames().frames.size();
    frame_pos_ += pos_inc;
    frame_pos_ -= std::floor(frame_pos_);
}
}