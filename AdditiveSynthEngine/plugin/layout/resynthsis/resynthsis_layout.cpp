#include "resynthsis_layout.h"

#include <format>
#include <AudioFile.h>
#include "param/resynthsis_param.h"
#include "layout/gui_param_pack.h"
#include "utli/convert.h"
#include "engine/synth.h"
#include "engine/resynthsis/image_base.h"

template<>
struct std::formatter<juce::String, char> {
    auto parse(format_parse_context& ctx)
    {
        const auto i = ctx.begin();
        const auto end = ctx.end();
        // 在这个函数里面解析字符串, 记录有关信息
        if (i == end)
        {
            // 说明是{}这样的占位符
            return end;
        }
        else {
            throw std::format_error("Cannot understand it.");
            return i;
        }
    }

    template <typename OutputIt>
    auto format(const juce::String& value, std::basic_format_context<OutputIt, char> &ctx) const noexcept
    {
        auto output = ctx.out();
        std::format_to(output, "{}", value.toStdString());
        return output;
    }
};

namespace mana {
class JuceImageAdpter : public ImageBase {
public:
    JuceImageAdpter(const juce::Image& img) : img_(img) {}
    int GetHeight() const override { return img_.getHeight(); }
    int GetWidth() const override { return img_.getWidth(); }
    SimplePixel GetPixel(int x, int y) const override {
        auto color = img_.getPixelAt(x, y);
        return SimplePixel{ color.getRed(), color.getGreen(), color.getBlue() };
    }
private:
    const juce::Image img_;
};
}

namespace mana {
ResynthsisLayout::ResynthsisLayout(Synth& synth)
    : synth_(synth) {
    is_enable_ = std::make_unique<WrapCheckBox>(synth.GetParamBank().GetParamPtr<BoolParameter>("resynthsis.enable"));
    is_enable_->addListener(this);
    for (int i = 0; auto & k : arg_knobs_) {
        k = std::make_unique<WrapSlider>(synth.GetParamBank().GetParamPtr(std::format("resynthsis.arg{}", i++)));
    }

    SetGuiKnobs(arg_knobs_,
                param::Resynthsis_FormantMix{},
                param::Resynthsis_FormantShift{},
                param::Resynthsis_FrameOffset{},
                param::Resynthsis_FrameSpeed{},
                param::Resynthsis_FreqScale{},
                param::Resynthsis_GainMix{});

    is_formant_remap_ = std::make_unique<WrapCheckBox>(synth.GetParamBank().GetParamPtr<BoolParameter>("resynthsis.formant_remap"));

    image_view_ = std::make_unique<juce::ImageComponent>();
    image_view_->setImagePlacement(juce::RectanglePlacement::stretchToFit);
    audio_button_ = std::make_unique<juce::TextButton>("audio", "open audio file");
    image_button_ = std::make_unique<juce::TextButton>("image", "open image file");
    audio_button_->addListener(this);
    image_button_->addListener(this);

    audio_file_chooser_ = std::make_unique<juce::FileChooser>("open audio",
                                                              juce::File{},
                                                              "*.wav");
    image_file_chooser_ = std::make_unique<juce::FileChooser>("open image",
                                                              juce::File{},
                                                              "*.png;*.jpg;*.jpeg");

    // add
    addAndMakeVisible(is_enable_.get());
    addAndMakeVisible(is_formant_remap_.get());
    addAndMakeVisible(audio_button_.get());
    addAndMakeVisible(image_button_.get());
    for (auto & k : arg_knobs_) {
        addAndMakeVisible(k.get());
    }
    addAndMakeVisible(image_view_.get());
}

void ResynthsisLayout::paintOverChildren(juce::Graphics& g) {
    float cursor_pos = synth_.GetDisplayOscillor().GetResynthsisProcessor().GetPlayerPosition();
    auto cursor_x = static_cast<int>(cursor_pos * getWidth());
    g.setColour(juce::Colours::white);
    g.drawVerticalLine(cursor_x, 50.0f + 16.0f, static_cast<float>(getHeight()));
}

void ResynthsisLayout::resized() {
    is_enable_->setBounds(0, 0, 100, 16);
    is_formant_remap_->setBounds(100, 0, 100, 16);
    audio_button_->setBounds(200, 0, 100, 16);
    image_button_->setBounds(300, 0, 100, 16);
    for (int i = 0; auto & k : arg_knobs_) {
        k->setBounds(50 * i, 16, 50, 50);
        ++i;
    }
    image_view_->setBounds(0, 16 + 50, getWidth(), getHeight() - 16 - 50);
}

void ResynthsisLayout::CreateAudioResynthsis(const juce::String& path) {
    auto task = [file_path = path.toStdString(), work_id = ++resynthsis_work_counter_, this] {
        ResynthsisFrames frame;
        AudioFile<float> audio_file_;
        if (!audio_file_.load(file_path) || audio_file_.samples.empty()) {
            DBG(std::format(R"([resynthsis]: load file "{}" *failed*)", file_path));
            return;
        }

        frame = synth_.CreateResynthsisFramesFromAudio(audio_file_.samples.at(0),
                                                       static_cast<float>(audio_file_.getSampleRate()));
        DBG(std::format(R"([resynthsis]: load file "{}" *success*)", file_path));

        if (resynthsis_work_counter_.load() != work_id) {
            DBG(std::format(R"([resynthsis]: file "{}" load *canceled*)", file_path));
            return;
        }

        // turn audio data into img
        const auto& spectrum = frame.frames;
        const auto display_height = kNumPartials;// hide half fft spectrum and image because it only used in formant stretch
        const auto display_width = static_cast<int>(spectrum.size());// hide half fft spectrum and image because it only used in formant stretch
        juce::Image img{ juce::Image::RGB, display_width, display_height,false };
        for (int x = 0; x < display_width; ++x) {
            const auto& draw_frame = spectrum.at(x);

            for (int y = 0; y < display_height; ++y) {
                auto y_idx = display_height - 1 - y;

                // map gain to g
                auto db_g = utli::GainToDb<-60.0f>(draw_frame.gains[y_idx]) / 60.0f + 1.0f;
                auto g = static_cast<unsigned char>(std::clamp(0xff * db_g, 0.0f, 255.0f));

                // map ratio_diff to b
                auto fre_diff = draw_frame.ratio_diffs[y_idx];
                auto bit_fre_diff = std::clamp(fre_diff, -1.0f, 1.0f);
                auto nor_fre_diff = 0.5f + 0.5f * bit_fre_diff;
                auto b = static_cast<unsigned char>(std::clamp(0xff * nor_fre_diff + 0.5f, 0.0f, 255.0f));

                img.setPixelAt(x, y, juce::Colour(0, g, b));
            }
        }

        {
            std::scoped_lock lock{ synth_.GetSynthLock() };
            synth_.SetResynthsisFrames(std::move(frame));
        }

        {
            /*std::scoped_lock lock{ ui_lock_ };*/
            //render_img_ = std::move(img);
            const juce::MessageManagerLock lock{};
            image_view_->setImage(img);
        }
    };
    std::thread{ std::move(task) }.detach();
}

void ResynthsisLayout::CreateImageResynthsis(const juce::String& path) {
    auto task = [file_path = path, work_id = ++resynthsis_work_counter_, this] {
        ResynthsisFrames frame;

        auto img = juce::ImageFileFormat::loadFrom(juce::File(file_path));
        if (!img.isValid()) {
            DBG(std::format(R"([resynthsis]: load file "{}" *failed*)", file_path));
            return;
        }

        frame = synth_.CreateResynthsisFramesFromImage(std::make_unique<JuceImageAdpter>(img));
        std::clog << std::format(R"([resynthsis]: load file "{}" *success*)", file_path) << std::endl;

        if (resynthsis_work_counter_.load() != work_id) {
            std::clog << std::format(R"([resynthsis]: file "{}" load *canceled*)", file_path) << std::endl;
            return;
        }

        // turn audio data into img
        const auto& spectrum = frame.frames;
        const auto display_height = kNumPartials;// hide half fft spectrum and image because it only used in formant stretch
        const auto display_width = static_cast<int>(spectrum.size());// hide half fft spectrum and image because it only used in formant stretch
        juce::Image resynthsis_img{ juce::Image::RGB, display_width, display_height,false };
        for (int x = 0; x < display_width; ++x) {
            const auto& draw_frame = spectrum.at(x);

            for (int y = 0; y < display_height; ++y) {
                auto y_idx = display_height - 1 - y;

                // map gain to g
                auto db_g = utli::GainToDb<-60.0f>(draw_frame.gains[y_idx]) / 60.0f + 1.0f;
                auto g = static_cast<unsigned char>(std::clamp(0xff * db_g, 0.0f, 255.0f));

                // map ratio_diff to b
                auto fre_diff = draw_frame.ratio_diffs[y_idx];
                auto bit_fre_diff = std::clamp(fre_diff, -1.0f, 1.0f);
                auto nor_fre_diff = 0.5f + 0.5f * bit_fre_diff;
                auto b = static_cast<unsigned char>(std::clamp(0xff * nor_fre_diff + 0.5f, 0.0f, 255.0f));

                resynthsis_img.setPixelAt(x, y, juce::Colour(0, g, b));
            }
        }

        {
            std::scoped_lock lock{ synth_.GetSynthLock() };
            synth_.SetResynthsisFrames(std::move(frame));
        }

        {
            const juce::MessageManagerLock lock{};
            image_view_->setImage(resynthsis_img);
        }
    };
    std::thread{ std::move(task) }.detach();
}

void ResynthsisLayout::buttonClicked(juce::Button* ptr_button) {
    if (ptr_button == audio_button_.get()) {
        audio_file_chooser_->launchAsync(
            juce::FileBrowserComponent::FileChooserFlags::openMode,
            [this](const juce::FileChooser& chooser) {
            if (chooser.getResults().isEmpty()) {
                return;
            }
            auto load_file = chooser.getResult();
            CreateAudioResynthsis(load_file.getFullPathName());
        });
    }
    else if (ptr_button == image_button_.get()) {
        image_file_chooser_->launchAsync(
            juce::FileBrowserComponent::FileChooserFlags::openMode,
            [this](const juce::FileChooser& chooser) {
            if (chooser.getResults().isEmpty()) {
                return;
            }
            auto load_file = chooser.getResult();
            CreateImageResynthsis(load_file.getFullPathName());
        });
    }
    else if (ptr_button == is_enable_.get()) {
        if (is_enable_->getToggleState()) {
            startTimerHz(10);
        }
        else {
            stopTimer();
        }
    }
}

void ResynthsisLayout::timerCallback() {
    repaint();
}

void ResynthsisLayout::BeginHighlightModulator(std::string_view id) {
    for (const auto& p : arg_knobs_) {
        p->BeginHighlightModulator(id);
    }
}

void ResynthsisLayout::StopHighliteModulator() {
    for (const auto& p : arg_knobs_) {
        p->StopHighliteModulator();
    }
}
}