#include "filter_route_layout.h"

#include "engine/synth.h"

namespace mana {
FilterRouteLayout::FilterRouteLayout(Synth& synth) {
    auto& bank = synth.GetParamBank();

    filter1_enable_ = std::make_unique<WrapCheckBox>(bank.GetParamPtr<BoolParameter>("filter0.enable"));
    filter2_enable_ = std::make_unique<WrapCheckBox>(bank.GetParamPtr<BoolParameter>("filter1.enable"));
    stream_type_ = std::make_unique<WrapCheckBox>(bank.GetParamPtr<BoolParameter>("filter.stream_type"));

    addAndMakeVisible(filter1_enable_.get());
    addAndMakeVisible(filter2_enable_.get());
    addAndMakeVisible(stream_type_.get());
}

void FilterRouteLayout::resized() {
    filter1_enable_->setBounds(0, 0, 16, 16);
    filter2_enable_->setBounds(0, 16, 16, 16);
    stream_type_->setBounds(0, 32, 16, 16);
}
}