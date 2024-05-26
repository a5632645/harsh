#pragma once

#include <array>
#include "engine/forward_decalre.h"
#include "ui/wrap_slider.h"
#include "utli/spin_lock.h"
#include "ui/wrap_check_box.h"
#include "ui/wrap_drop_box.h"
#include "layout/modu_container.h"
#include <juce_gui_basics/juce_gui_basics.h>

namespace mana {
class ResynthsisLayout : public ModuContainer, public juce::Component, private juce::FileDragAndDropTarget {
public:
    ResynthsisLayout(Synth& synth);

    void paint(juce::Graphics& g) override;
    void resized() override;
private:
    void CreateAudioResynthsis(const juce::String& path);
    void CreateImageResynthsis(const juce::String& path);

    Synth& synth_;
    std::unique_ptr<WrapCheckBox> is_enable_;
    std::unique_ptr<WrapCheckBox> is_formant_remap_;
    std::array<std::unique_ptr<WrapSlider>, 7> arg_knobs_;
    std::atomic<int> resynthsis_work_counter_;
    utli::SpinLock ui_lock_;
    juce::Image render_img_;

    // 通过 FileDragAndDropTarget 继承
    bool isInterestedInFileDrag(const juce::StringArray& files) override;
    void filesDropped(const juce::StringArray& files, int x, int y) override;
};
}