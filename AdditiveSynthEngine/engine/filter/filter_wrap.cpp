#include "filter_wrap.h"

namespace mana {
FilterWrap::FilterWrap()
    : filter1_(0)
    , filter2_(1) {
    filter_out_.resize(kNumPartials);
}

void FilterWrap::Init(float sample_rate, float update_rate) {
    filter1_.Init(sample_rate, update_rate);
    filter2_.Init(sample_rate, update_rate);
}

void FilterWrap::PrepareParams(OscillorParams& params) {
    filter1_.PrepareParams(params);
    filter2_.PrepareParams(params);

    stream_type_ = params.GetParam<BoolParameter>("filter.stream_type");
    filter1_enable_ = params.GetParam<BoolParameter>("filter0.enable");
    filter2_enable_ = params.GetParam<BoolParameter>("filter1.enable");
}

void FilterWrap::Process(Partials& partials) {
    auto enable1 = filter1_enable_->GetBool();
    auto enable2 = filter2_enable_->GetBool();

    if (!enable1 && !enable2) {
        return;
    }

    if (enable1 && !enable2) {
        std::ranges::transform(partials.gains, filter1_.Process(partials), partials.gains.begin(), std::multiplies{});
        return;
    }

    if (enable2 && !enable1) {
        std::ranges::transform(partials.gains, filter2_.Process(partials), partials.gains.begin(), std::multiplies{});
        return;
    }

    const auto& filter1_out = filter1_.Process(partials);
    const auto& filter2_out = filter2_.Process(partials);

    if (stream_type_->GetBool()) { // seriel
        std::ranges::transform(filter1_out, filter2_out, filter_out_.begin(), std::multiplies{});
    }
    else { // parallel
        std::ranges::transform(filter1_out, filter2_out, filter_out_.begin(), std::plus{});
    }

    std::ranges::transform(partials.gains, filter_out_, partials.gains.begin(), std::multiplies{});
}

void FilterWrap::OnUpdateTick() {
    filter1_.OnUpdateTick();
    filter2_.OnUpdateTick();
}

void FilterWrap::OnNoteOn(int note) {
    filter1_.OnNoteOn(note);
    filter2_.OnNoteOn(note);
}

void FilterWrap::OnNoteOff() {
    filter1_.OnNoteOff();
    filter2_.OnNoteOff();
}
}