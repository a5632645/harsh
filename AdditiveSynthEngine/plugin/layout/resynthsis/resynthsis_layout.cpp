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

    int GetMaxGreen() const override {
        auto max = 0;
        const auto w = img_.getWidth();
        const auto h = img_.getHeight();
        for (int i = 0; i < w; ++i) {
            for (int j = 0; j < h; ++j) {
                max = std::max(max, static_cast<int>(img_.getPixelAt(i, j).getGreen()));
            }
        }
        return max;
    }
private:
    const juce::Image img_;
};
}

namespace mana {
ResynthsisLayout::ResynthsisLayout(Synth& synth)
    : synth_(synth) {
    is_enable_ = std::make_unique<WrapCheckBox>(synth.GetParamBank().GetParamPtr<BoolParameter>("resynthsis.enable"));
    is_enable_->onStateChange = [this]() {
        if (is_enable_->getToggleState()) {
            startTimerHz(10);
        }
        else {
            stopTimer();
        }
    };

    auto& bank = synth.GetSynthParams().GetParamBank();
    arg_knobs_.emplace_back(std::make_unique<WrapSlider>(bank.GetParamPtr("resynthsis.freq_scale")));
    arg_knobs_.emplace_back(std::make_unique<WrapSlider>(bank.GetParamPtr("resynthsis.start_offset")));
    arg_knobs_.emplace_back(std::make_unique<WrapSlider>(bank.GetParamPtr("resynthsis.speed")));
    arg_knobs_.emplace_back(std::make_unique<WrapSlider>(bank.GetParamPtr("resynthsis.speedx")));
    arg_knobs_.emplace_back(std::make_unique<WrapSlider>(bank.GetParamPtr("resynthsis.formant_mix")));
    arg_knobs_.emplace_back(std::make_unique<WrapSlider>(bank.GetParamPtr("resynthsis.formant_shift")));
    arg_knobs_.emplace_back(std::make_unique<WrapSlider>(bank.GetParamPtr("resynthsis.gain_mix")));
    arg_knobs_.emplace_back(std::make_unique<WrapSlider>(bank.GetParamPtr("resynthsis.start_range")));

    image_view_ = std::make_unique<juce::ImageComponent>();
    image_view_->setImagePlacement(juce::RectanglePlacement::stretchToFit);
    audio_button_ = std::make_unique<juce::TextButton>("audio", "open audio file");
    image_button_ = std::make_unique<juce::TextButton>("image", "open image file");
    audio_button_->addListener(this);
    image_button_->addListener(this);
    image_export_button_ = std::make_unique<juce::TextButton>("export image");
    image_export_button_->addListener(this);
    addAndMakeVisible(image_export_button_.get());

    audio_file_chooser_ = std::make_unique<juce::FileChooser>("open audio",
                                                              juce::File{},
                                                              "*.wav");
    image_file_chooser_ = std::make_unique<juce::FileChooser>("open image",
                                                              juce::File{},
                                                              "*.png;*.jpg;*.jpeg");

    // add
    addAndMakeVisible(is_enable_.get());
    addAndMakeVisible(audio_button_.get());
    addAndMakeVisible(image_button_.get());
    for (auto & k : arg_knobs_) {
        addAndMakeVisible(k.get());
    }
    addAndMakeVisible(image_view_.get());

    // init
    if (synth_.IsResynthsisAvailable()) {
        std::thread([this] {
            auto img = GenerateResynsisImage(synth_.GetResynthsisFrames());
            {
                const juce::MessageManagerLock lock{};
                image_view_->setImage(img);
            }
        }).detach();
    }
    is_enable_->onStateChange();
}

void ResynthsisLayout::paintOverChildren(juce::Graphics& g) {
    if (!synth_.IsResynthsisAvailable())
        return;
    auto cursor_pos = synth_.GetDisplayOscillor().GetResynthsisProcessor().GetPartialPositions();

    auto partial_height = static_cast<float>(image_view_->getHeight()) / static_cast<float>(kNumPartials);
    auto partial_y = static_cast<float>(image_view_->getBottom());
    g.setColour(juce::Colours::black);
    for (int i = 0; i < kNumPartials; ++i) {
        auto cursor_x = cursor_pos[i] * getWidth();
        auto cursor_bound = juce::Rectangle{ cursor_x, partial_y - partial_height, 3.0f, partial_height };
        g.fillRect(cursor_bound);
        partial_y -= partial_height;
    }
}

void ResynthsisLayout::resized() {
    is_enable_->setBounds(0, 0, 100, 16);
    audio_button_->setBounds(200, 0, 100, 16);
    image_button_->setBounds(300, 0, 100, 16);
    image_export_button_->setBounds(400, 0, 100, 16);
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
        auto img = GenerateResynsisImage(frame);

        {
            std::scoped_lock lock{ synth_.GetSynthLock() };
            synth_.SetResynthsisFrames(std::move(frame));
        }

        {
            const juce::MessageManagerLock lock{};
            image_view_->setImage(img);
        }
    };
    std::thread{ std::move(task) }.detach();
}

void ResynthsisLayout::CreateImageResynthsis(const juce::String& path, bool stretch_image) {
    auto task = [file_path = path, work_id = ++resynthsis_work_counter_, stretch_image, this] {
        ResynthsisFrames frame;

        auto img = juce::ImageFileFormat::loadFrom(juce::File(file_path));
        if (!img.isValid()) {
            DBG(std::format(R"([resynthsis]: load file "{}" *failed*)", file_path));
            return;
        }

        frame = synth_.CreateResynthsisFramesFromImage(std::make_unique<JuceImageAdpter>(img), stretch_image);
        std::clog << std::format(R"([resynthsis]: load file "{}" *success*)", file_path) << std::endl;

        if (resynthsis_work_counter_.load() != work_id) {
            std::clog << std::format(R"([resynthsis]: file "{}" load *canceled*)", file_path) << std::endl;
            return;
        }

        // turn audio data into img
        auto resynthsis_img = GenerateResynsisImage(frame);

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

juce::Image ResynthsisLayout::GenerateResynsisImage(ResynthsisFrames& frames) {
    // turn audio data into img
    const auto& spectrum = frames.frames;
    const auto display_height = kNumPartials;
    const auto display_width = frames.num_frame;
    juce::Image resynthsis_img{ juce::Image::RGB, display_width, display_height,false };
    auto should_6dB_level_up = frames.source_type == ResynthsisFrames::Type::kImage;

    constexpr auto db6up_table = []() {
        std::array<float, kNumPartials> out{};
        for (int i = 0; i < kNumPartials; ++i)
            out[i] = utli::cp::GainToDb(i + 1.0L, -300.0L);
        return out;
    }();
    constexpr auto min_db = -60.0f;
    constexpr auto max_db = 0.0f;

    for (int x = 0; x < display_width; ++x) {
        const auto& draw_frame = spectrum.at(x);

        for (int y = 0; y < display_height; ++y) {
            auto y_idx = display_height - 1 - y;

            // map gain to g
            auto db_g = draw_frame.db_gains[y_idx];
            if (should_6dB_level_up) {
                db_g += (db6up_table[y_idx] - frames.level_up_db);
            }
            auto nor_db_g = (db_g - min_db) / (max_db - min_db);
            auto g = static_cast<unsigned char>(std::clamp(0xff * nor_db_g, 0.0f, 255.0f));

            // map ratio_diff to b
            auto fre_diff = draw_frame.ratio_diffs[y_idx];
            auto bit_fre_diff = std::clamp(fre_diff, -1.0f, 1.0f);
            auto nor_fre_diff = 0.5f + 0.5f * bit_fre_diff;
            auto b = static_cast<unsigned char>(std::clamp(0xff * nor_fre_diff, 0.0f, 255.0f));

            resynthsis_img.setPixelAt(x, y, juce::Colour(0, g, b));
        }
    }
    return resynthsis_img;
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
            image_post_process_selector_ = std::make_unique<juce::AlertWindow>("image stretch",
                                                                               "do you want to stretch the image?",
                                                                               juce::MessageBoxIconType::QuestionIcon);
            image_post_process_selector_->addButton("stretch", 0);
            image_post_process_selector_->addButton("keep", 1);
            image_post_process_selector_->enterModalState(true, juce::ModalCallbackFunction::create([this, file = load_file](int w) {
                image_post_process_selector_->exitModalState(w);
                image_post_process_selector_->setVisible(false);
                CreateImageResynthsis(file.getFullPathName(), w == 0);
            }));
        });
    }
    else if (ptr_button == image_export_button_.get()) {
        if (!image_view_->getImage().isValid()) {
            return;
        }
        auto popup = new juce::FileChooser("export image", juce::File{}, "*.png");
        popup->launchAsync(juce::FileBrowserComponent::FileChooserFlags::saveMode,
                           [popup, img = image_view_->getImage()](const juce::FileChooser& c) {
            auto _ = std::unique_ptr<juce::FileChooser>(popup);
            if (c.getResult() == juce::File{}) {
                return;
            }
            auto ff = c.getResult();
            juce::PNGImageFormat f;
            if (ff.exists()) {
                ff.deleteFile();
            }
            juce::FileOutputStream s{ ff };
            f.writeImageToStream(img, s);
        });
    }
}

void ResynthsisLayout::timerCallback() {
    repaint();
}
}