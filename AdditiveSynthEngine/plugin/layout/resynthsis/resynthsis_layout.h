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
class ResynthsisLayout : public ModuContainer,
    public juce::Component,
    private juce::Button::Listener,
    private juce::Timer {
public:
    ResynthsisLayout(Synth& synth);
    ~ResynthsisLayout() override { stopTimer(); }

    void paintOverChildren(juce::Graphics& g) override;
    void resized() override;

    // 通过 ModuContainer 继承
    void BeginHighlightModulator(std::string_view id) override;
    void StopHighliteModulator() override;
private:
    void CreateAudioResynthsis(const juce::String& path);
    void CreateImageResynthsis(const juce::String& path);

    Synth& synth_;
    std::unique_ptr<WrapCheckBox> is_enable_;
    std::unique_ptr<WrapCheckBox> is_formant_remap_;
    std::array<std::unique_ptr<WrapSlider>, 7> arg_knobs_;
    std::atomic<int> resynthsis_work_counter_;
    std::unique_ptr<juce::ImageComponent> image_view_;

    std::unique_ptr<juce::TextButton> audio_button_;
    std::unique_ptr<juce::TextButton> image_button_;
    std::unique_ptr<juce::FileChooser> audio_file_chooser_;
    std::unique_ptr<juce::FileChooser> image_file_chooser_;

    // 通过 Listener 继承
    void buttonClicked(juce::Button*) override;

    // 通过 Timer 继承
    void timerCallback() override;
};
}