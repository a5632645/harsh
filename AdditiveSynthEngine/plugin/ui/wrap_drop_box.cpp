#include "wrap_drop_box.h"

#include "data/juce_param.h"

namespace mana {
WrapDropBox::WrapDropBox(IntChoiceParameter* p)
    : parameter_(*p) {
    addItemList(parameter_.As<JuceEnumParam>().GetRef().choices, 1);
    attachment_ = std::make_unique<juce::ComboBoxParameterAttachment>(parameter_.As<JuceEnumParam>().GetRef(), *this);
}

WrapDropBox::~WrapDropBox() {
    attachment_ = nullptr;
}
}