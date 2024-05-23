#include "resynthsis_layout.h"

#include <format>
#include <AudioFile.h>
#include "param/resynthsis_param.h"
#include "layout/gui_param_pack.h"
#include "utli/convert.h"
#include "engine/synth.h"

namespace mana {
ResynthsisLayout::ResynthsisLayout(Synth& synth)
    : synth_(synth)
    , is_enable_(synth.GetParamBank().GetParamPtr<BoolParameter>("resynthsis.enable")) {
    for (int i = 0; auto & k : arg_knobs_) {
        k.set_parameter(synth.GetParamBank().GetParamPtr(std::format("resynthsis.arg{}", i++)));
    }

    SetGuiKnobs(arg_knobs_,
                param::Resynthsis_FormantMix{},
                param::Resynthsis_FormantShift{},
                param::Resynthsis_FrameOffset{},
                param::Resynthsis_FrameSpeed{},
                param::Resynthsis_FreqScale{},
                param::Resynthsis_GainMix{});

    is_enable_.SetText("resynthsis");

    is_formant_remap_.SetText("fm.remap");
    is_formant_remap_.SetParameter(synth.GetParamBank().GetParamPtr<BoolParameter>("resynthsis.formant_remap"));
}

void ResynthsisLayout::Paint() {
    CheckAndDoResynthsis();

    is_enable_.Paint();
    if (!is_enable_.IsChecked()) {
        return;
    }

    DrawSpectrum();
    std::ranges::for_each(arg_knobs_, &Knob::display);
    is_formant_remap_.Paint();
}

void ResynthsisLayout::SetBounds(int x, int y, int w, int h) {
    auto x_f = static_cast<float>(x);
    auto y_f = static_cast<float>(y);
    auto w_f = static_cast<float>(w);
    is_enable_.SetBounds(rgc::Bounds(x_f, y_f, 12.0f, 12.0f));
    bound_ = rgc::Bounds{ x_f,y_f + 50.0f + 12.0f,w_f,h - 50.0f - 12.0f };

    for (int i = 0; auto & k : arg_knobs_) {
        k.set_bound(x + 50 * i, y + 12, 50, 50);
        ++i;
    }
    is_formant_remap_.SetBounds({ x + 50.0f * arg_knobs_.size(),y + 12.0f,16.0f,16.0f });
}

void ResynthsisLayout::CheckAndDoResynthsis() {
    if (IsFileDropped()) {
        FilePathList files = ::LoadDroppedFiles();
        std::vector<std::string> output(files.paths, files.paths + files.count);
        ::UnloadDroppedFiles(files);

        auto input_file = std::ranges::find_if(output, [](const std::string& file_name) {
            return file_name.ends_with(".wav")
                || file_name.ends_with(".png")
                || file_name.ends_with(".jpg")
                || file_name.ends_with(".bmp")
                || file_name.ends_with(".jpeg");
        });
        if (input_file == output.end()) {
            return;
        }

        auto task = [file_path = *input_file, work_id = ++resynthsis_work_counter_, this] {
            ResynthsisFrames frame;
            if (file_path.ends_with(".wav")) {
                AudioFile<float> audio_file_;
                if (!audio_file_.load(file_path) || audio_file_.samples.empty()) {
                    std::cerr << std::format(R"([resynthsis]: load file "{}" *failed*)", file_path) << std::endl;
                    return;
                }

                frame = synth_.CreateResynthsisFramesFromAudio(audio_file_.samples.at(0),
                                                               static_cast<float>(audio_file_.getSampleRate()));
                std::clog << std::format(R"([resynthsis]: load file "{}" *success*)", file_path) << std::endl;
            }
            else {
                try {
                    raylib::Image img{ file_path };
                    std::vector<std::vector<SimplePixel>> image_in;
                    auto w = img.GetWidth();
                    auto h = img.GetHeight();
                    image_in.resize(w);
                    for (auto& v : image_in) { v.resize(h); }

                    for (int x = 0; auto & line : image_in) {
                        for (int y = 0; auto & c : line) {
                            auto raylib_color = img.GetColor(x, y);
                            c.r = raylib_color.r;
                            c.g = raylib_color.g;
                            c.b = raylib_color.b;

                            ++y;
                        }
                        ++x;
                    }
                    frame = synth_.CreateResynthsisFramesFromImage(image_in);
                    std::clog << std::format(R"([resynthsis]: load file "{}" *success*)", file_path) << std::endl;
                }
                catch (...) {
                    std::cerr << std::format(R"([resynthsis]: load file "{}" *failed*)", file_path) << std::endl;
                    return;
                }
            }

            if (resynthsis_work_counter_.load() != work_id) {
                std::clog << std::format(R"([resynthsis]: file "{}" load *canceled*)", file_path) << std::endl;
                return;
            }

            // turn audio data into img
            auto w = static_cast<int>(bound_.GetWidth());
            auto h = static_cast<int>(bound_.GetHeight());
            std::vector<std::vector<Color>> img;
            img.resize(w);
            for (auto& v : img) { v.resize(h); }

            const auto& spectrum = frame.frames;
            const auto display_height = kNumPartials;// hide half fft spectrum and image because it only used in formant stretch
            for (int x = 0; x < w; ++x) {
                auto x_idx = static_cast<size_t>(x / bound_.GetWidth() * spectrum.size());
                const auto& draw_frame = spectrum.at(x_idx);

                for (int y = 0; y < h; ++y) {
                    auto y_idx = static_cast<size_t>((1.0f - y / bound_.GetHeight()) * (display_height - 1));

                    // map gain to g
                    auto db_g = utli::GainToDb<-60.0f>(draw_frame.gains[y_idx]) / 60.0f + 1.0f;
                    auto g = static_cast<unsigned char>(std::clamp(0xff * db_g, 0.0f, 255.0f));

                    // map ratio_diff to b
                    auto fre_diff = draw_frame.ratio_diffs[y_idx];
                    auto bit_fre_diff = std::clamp(fre_diff, -1.0f, 1.0f);
                    auto nor_fre_diff = 0.5f + 0.5f * bit_fre_diff;
                    auto b = static_cast<unsigned char>(std::clamp(0xff * nor_fre_diff + 0.5f, 0.0f, 255.0f));

                    img[x][y] = Color(0, g, b, 255);
                }
            }

            {
                std::scoped_lock lock{ synth_.GetSynthLock() };
                synth_.SetResynthsisFrames(std::move(frame));
            }

            {
                std::scoped_lock lock{ ui_lock_ };
                render_img_.swap(img);
            }
        };
        std::thread{ std::move(task) }.detach();
    }
}

void ResynthsisLayout::DrawSpectrum() {
    std::scoped_lock lock{ ui_lock_ };

    auto tf_x = static_cast<int>(bound_.GetX());
    auto tf_y = static_cast<int>(bound_.GetY());

    for (int x = 0; auto & v : render_img_) {
        for (int y = 0; auto & c : v) {
            DrawPixel(tf_x + x, tf_y + y, c);
            ++y;
        }
        ++x;
    }

    float cursor_pos = synth_.GetDisplayOscillor().GetResynthsisProcessor().GetPlayerPosition();
    auto cursor_x = static_cast<int>(cursor_pos * bound_.GetWidth() + bound_.GetX());
    auto cursor_end = static_cast<int>(bound_.GetHeight() + bound_.GetY());
    DrawLine(cursor_x, tf_y, cursor_x, cursor_end, WHITE);
}
}