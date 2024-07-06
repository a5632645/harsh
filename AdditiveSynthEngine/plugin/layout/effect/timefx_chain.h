#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <vector>
#include <memory>
#include <engine/forward_decalre.h>
#include <engine/time_effect/fx_chain.h>

namespace mana {
class TimeFxChain : public juce::Component, private FxChain::Listener {
public:
    TimeFxChain(Synth& synth);
    ~TimeFxChain() override;
    void resized() override;
private:
    void MoveUp(int index);
    void MoveDown(int index);
    void mouseDrag(const juce::MouseEvent& e) override;
    void mouseDown(const juce::MouseEvent& e) override;
    void mouseUp(const juce::MouseEvent& e) override;
    bool IsFirstSection(int i) { return i == 0; }
    bool IsLastSection(int i) { return i == static_cast<int>(order_.size() - 1); }

    std::vector<juce::Component*> order_;
    std::vector<std::unique_ptr<juce::Component>> components_;
    juce::ComponentDragger dragger_;
    FxChain& fxchain_;

    // 通过 Listener 继承
    void OnReload(FxChain* pchain) override;
};
}