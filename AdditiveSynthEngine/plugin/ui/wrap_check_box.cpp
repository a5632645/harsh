#include "wrap_check_box.h"

#include "data/juce_param.h"

namespace mana {
WrapCheckBox::WrapCheckBox(BoolParameter* p)
    : parameter_(*p) {
    attachment_ = std::make_unique<juce::ButtonParameterAttachment>(parameter_.As<JuceBoolParam>().GetRef(), *this);
}

WrapCheckBox::~WrapCheckBox() {
    attachment_ = nullptr;
}
}