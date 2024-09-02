#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <array>
#include "engine/forward_decalre.h"
#include "ui/wrap_slider.h"
#include "utli/spin_lock.h"
#include "ui/wrap_check_box.h"
#include "ui/wrap_drop_box.h"
#include "audio_option_window.h"

namespace mana {
class ResynthsisLayout : public juce::Component,
    private juce::Button::Listener,
    private juce::Timer {
public:
    ResynthsisLayout(Synth& synth);
    ~ResynthsisLayout() override { stopTimer(); }

    void paintOverChildren(juce::Graphics& g) override;
    void resized() override;
private:
    void CreateAudioResynthsis(const juce::String& path, ResynthsisOption option);
    void CreateImageResynthsis(const juce::String& path, bool stretch_image);
    static juce::Image GenerateResynsisImage(ResynthsisFrames& frames);

    Synth& synth_;
    std::unique_ptr<WrapCheckBox> is_enable_;
    std::vector<std::unique_ptr<WrapSlider>> arg_knobs_;
    std::atomic<int> resynthsis_work_counter_;
    std::unique_ptr<juce::ImageComponent> image_view_;

    std::unique_ptr<juce::TextButton> audio_button_;
    std::unique_ptr<juce::TextButton> image_button_;
    std::unique_ptr<juce::TextButton> image_export_button_;
    std::unique_ptr<juce::FileChooser> audio_file_chooser_;
    std::unique_ptr<juce::FileChooser> image_file_chooser_;
    std::unique_ptr<juce::AlertWindow> image_post_process_selector_;
    std::unique_ptr<juce::AlertWindow> audio_window_;
    std::unique_ptr<AudioOptionWindow> audio_post_window_;

    // 通过 Listener 继承
    void buttonClicked(juce::Button*) override;

    // 通过 Timer 继承
    void timerCallback() override;
};
}