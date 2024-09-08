#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "engine/resynthsis/resynthsis_option.h"
#include "utli/convert.h"

namespace mana {
class AudioOptionWindow : public juce::Component, public juce::Button::Listener {
public:
    AudioOptionWindow() {
        addAndMakeVisible(window_len_);
        addAndMakeVisible(frequency_resolution_);
        addAndMakeVisible(side_lobe_level_);
        addAndMakeVisible(peak_filter_level_);
        addAndMakeVisible(smooth_time_);
        addAndMakeVisible(custom_win_len_);
        addAndMakeVisible(custom_freq_res_);
        addAndMakeVisible(custom_side_lobe_level_);
        addAndMakeVisible(custom_peak_filter_level_);
        addAndMakeVisible(custom_smooth_time_);
        addAndMakeVisible(label_win_len_);
        addAndMakeVisible(label_freq_res_);
        addAndMakeVisible(label_side_lobe_level_);
        addAndMakeVisible(label_peak_filter_level_);
        addAndMakeVisible(label_smooth_time_);

        addAndMakeVisible(window_type_);
        addAndMakeVisible(label_win_type_);

        window_type_.addItemList({ "kKaiser", "kBlackman", "kTaylor" }, 1);
        window_type_.setSelectedId(1);

        custom_freq_res_.addListener(this);
        custom_win_len_.addListener(this);
        custom_side_lobe_level_.addListener(this);
        custom_peak_filter_level_.addListener(this);
        custom_smooth_time_.addListener(this);

        window_len_.setEnabled(false);
        window_len_.setRange(128, 4096, 1);
        window_len_.setValue(2048);
        frequency_resolution_.setEnabled(false);
        frequency_resolution_.setRange(16.0, 1024.0, 1.0);
        frequency_resolution_.setValue(utli::PitchToFreq(36.0f));
        side_lobe_level_.setEnabled(false);
        side_lobe_level_.setRange(30.0, 100.0, 1.0);
        side_lobe_level_.setValue(80.0f);
        peak_filter_level_.setEnabled(false);
        peak_filter_level_.setRange(-100.0, -20.0, 1.0);
        peak_filter_level_.setValue(-60.0f);
        smooth_time_.setEnabled(false);
        smooth_time_.setRange(0.0, 100.0, 1.0);
        smooth_time_.setValue(10.0f);

        setSize(400, 300);
    }

    void resized() override {
        auto b = getLocalBounds();
        auto b1 = b.removeFromTop(50);
        label_win_type_.setBounds(b1.removeFromLeft(100));
        window_type_.setBounds(b1);

        b1 = b.removeFromTop(50);
        label_win_len_.setBounds(b1.removeFromLeft(100));
        custom_win_len_.setBounds(b1.removeFromLeft(50));
        window_len_.setBounds(b1);

        b1 = b.removeFromTop(50);
        label_freq_res_.setBounds(b1.removeFromLeft(100));
        custom_freq_res_.setBounds(b1.removeFromLeft(50));
        frequency_resolution_.setBounds(b1);

        b1 = b.removeFromTop(50);
        label_side_lobe_level_.setBounds(b1.removeFromLeft(100));
        custom_side_lobe_level_.setBounds(b1.removeFromLeft(50));
        side_lobe_level_.setBounds(b1);

        b1 = b.removeFromTop(50);
        label_peak_filter_level_.setBounds(b1.removeFromLeft(100));
        custom_peak_filter_level_.setBounds(b1.removeFromLeft(50));
        peak_filter_level_.setBounds(b1);

        b1 = b.removeFromTop(50);
        label_smooth_time_.setBounds(b1.removeFromLeft(100));
        custom_smooth_time_.setBounds(b1.removeFromLeft(50));
        smooth_time_.setBounds(b1);
    }
    
    void buttonClicked(juce::Button* b) override {
        if (b == &custom_win_len_) {
            window_len_.setEnabled(custom_win_len_.getToggleState());
        }
        else if (b == &custom_freq_res_) {
            frequency_resolution_.setEnabled(custom_freq_res_.getToggleState());
        }
        else if (b == &custom_side_lobe_level_) {
            side_lobe_level_.setEnabled(custom_side_lobe_level_.getToggleState());
        }
        else if (b == &custom_peak_filter_level_) {
            peak_filter_level_.setEnabled(custom_peak_filter_level_.getToggleState());
        }
        else if (b == &custom_smooth_time_) {
            smooth_time_.setEnabled(custom_smooth_time_.getToggleState());
        }
    }

    ResynthsisOption GetOption() const {
        ResynthsisOption op{};
        op.window_len_ = window_len_.getValue();
        op.frequency_resolution_ = frequency_resolution_.getValue();
        op.side_lobe_level_ = side_lobe_level_.getValue();
        op.peak_filter_level_ = peak_filter_level_.getValue();
        op.smooth_time_ = smooth_time_.getValue();
        op.custom_win_len_ = custom_win_len_.getToggleState();
        op.custom_freq_res_ = custom_freq_res_.getToggleState();
        op.custom_side_lobe_level_ = custom_side_lobe_level_.getToggleState();
        op.custom_peak_filter_level_ = custom_peak_filter_level_.getToggleState();
        op.custom_smooth_time_ = custom_smooth_time_.getToggleState();
        op.window_ = static_cast<ResynthsisOption::Window>(window_type_.getSelectedItemIndex());
        return op;
    }

private:
    juce::Slider window_len_;
    juce::Slider frequency_resolution_;
    juce::Slider side_lobe_level_;
    juce::Slider peak_filter_level_;
    juce::Slider smooth_time_;

    juce::ToggleButton custom_win_len_;
    juce::ToggleButton custom_freq_res_;
    juce::ToggleButton custom_side_lobe_level_;
    juce::ToggleButton custom_peak_filter_level_;
    juce::ToggleButton custom_smooth_time_;

    juce::Label label_win_len_{"Window Length", "Window Length"};
    juce::Label label_freq_res_{"Frequency Resolution", "Frequency Resolution"};
    juce::Label label_side_lobe_level_{"Side Lobe Level", "Side Lobe Level"};
    juce::Label label_peak_filter_level_{"Peak Filter Level", "Peak Filter Level"};
    juce::Label label_smooth_time_{"Smooth Time", "Smooth Time"};

    juce::Label label_win_type_{"Window Type", "Window Type"};
    juce::ComboBox window_type_;
};
}