#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <engine/modulation/quantize_map.h>

namespace mana {
class QuantizeMapEditor
    : public juce::Component
    , public QuantizeMap::Listener {
public:
    ~QuantizeMapEditor();
private:
    // 通过 Listener 继承
    void OnReload(QuantizeMap * map) override;
    void OnPointChanged(QuantizeMap * map, int idx) override;
};
}