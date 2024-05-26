#pragma once

#include "engine/forward_decalre.h"
#include "ui/wrap_check_box.h"

namespace mana {
class FilterRouteLayout : public juce::Component {
public:
    FilterRouteLayout(Synth& synth);

    void resized();
private:
    std::unique_ptr<WrapCheckBox> filter1_enable_;
    std::unique_ptr<WrapCheckBox> filter2_enable_;
    std::unique_ptr<WrapCheckBox> stream_type_;
};
}