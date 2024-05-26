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

    // add
    addAndMakeVisible(is_enable_.get());
    addAndMakeVisible(is_formant_remap_.get());
    for (auto & k : arg_knobs_) {
        addAndMakeVisible(k.get());
    }
}

void ResynthsisLayout::paint(juce::Graphics& g) {
    std::scoped_lock lock{ ui_lock_ };

    g.drawImage(render_img_, getLocalBounds().toFloat());

    float cursor_pos = synth_.GetDisplayOscillor().GetResynthsisProcessor().GetPlayerPosition();
    auto cursor_x = static_cast<int>(cursor_pos * getWidth());
    g.setColour(juce::Colours::white);
    g.drawVerticalLine(cursor_x, 0.0f, static_cast<float>(getHeight()));
}

void ResynthsisLayout::resized() {
    is_enable_->setBounds(0, 0, 12.0f, 12.0f);

    for (int i = 0; auto & k : arg_knobs_) {
        k->setBounds(50 * i, 12, 50, 50);
        ++i;
    }
    is_formant_remap_->setBounds(50.0f * arg_knobs_.size(), 12.0f, 16.0f, 16.0f);
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
            std::scoped_lock lock{ ui_lock_ };
            render_img_ = std::move(img);
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
            ;
            std::clog << std::format(R"([resynthsis]: file "{}" load *canceled*)", file_path) << std::endl;
            return;
        }

        // turn audio data into img
        {
            std::scoped_lock lock{ synth_.GetSynthLock() };
            synth_.SetResynthsisFrames(std::move(frame));
        }

        {
            std::scoped_lock lock{ ui_lock_ };
            render_img_ = std::move(img);
        }
    };
    std::thread{ std::move(task) }.detach();
}

bool ResynthsisLayout::isInterestedInFileDrag(const juce::StringArray& files) {
    for (const auto& s : files) {
        if (s.endsWith(".wav")
            || s.endsWith(".png")
            || s.endsWith(".jpg")
            || s.endsWith(".bmp")
            || s.endsWith(".jpeg")) {
            return true;
        }
    }
    return false;
}

void ResynthsisLayout::filesDropped(const juce::StringArray& files, int x, int y) {
    for (const auto& s : files) {
        if (s.endsWith(".wav")) {
            CreateAudioResynthsis(s);
            return;
        }
        else if (s.endsWith(".png")
                 || s.endsWith(".jpg")
                 || s.endsWith(".bmp")
                 || s.endsWith(".jpeg")) {
            CreateImageResynthsis(s);
            return;
        }
    }
}
}