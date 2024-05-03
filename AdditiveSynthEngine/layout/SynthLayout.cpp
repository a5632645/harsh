#include "SynthLayout.h"

#include <AudioFile.h>
#include "param/standard_param.h"
#include "param/timber_param.h"
#include "param/param.h"

namespace mana {
SynthLayout::SynthLayout(Synth& synth)
    : synth_(synth)
    , scope_(synth)
    , dissonance_layout_(synth)
    , timber_layout_(synth)
    , filter_layout_(synth)
    , effect_layout0_(synth, 0)
    , effect_layout1_(synth, 1)
    , effect_layout2_(synth, 2)
    , effect_layout3_(synth, 3)
    , effect_layout4_(synth, 4)
    , resynthsis_layout_(synth)
    , standard_layout_(synth) {
}

void SynthLayout::paint() {
    timber_layout_.Paint();
    standard_layout_.Paint();
    scope_.Paint();
    dissonance_layout_.Paint();
    filter_layout_.Paint();
    effect_layout0_.Paint();
    effect_layout1_.Paint();
    effect_layout2_.Paint();
    effect_layout3_.Paint();
    effect_layout4_.Paint();
    resynthsis_layout_.Paint();

    if (IsFileDropped()) {
        FilePathList files = ::LoadDroppedFiles();
        std::vector<std::string> output(files.paths, files.paths + files.count);
        ::UnloadDroppedFiles(files);

        auto audio_file = std::ranges::find_if(output, [](const std::string& file_name) {
            return file_name.find(".wav") != std::string::npos;
        });
        if (audio_file == output.end()) {
            return;
        }

        AudioFile<float> audio_file_;
        if (!audio_file_.load(*audio_file) || audio_file_.samples.empty()) {
            std::cerr << std::format(R"([resynthsis]: load file "{}" *failed*)", *audio_file) << std::endl;
            return;
        }

        synth_.CreateResynthsisFrames(audio_file_.samples.at(0), audio_file_.getSampleRate());
        std::clog << std::format(R"([resynthsis]: load file "{}" *success*))", *audio_file) << std::endl;
    }
}

void SynthLayout::SetBounds(int x, int y, int w, int h) {
    timber_layout_.SetBounds(0, 0, 200, 12 + 70);
    scope_.SetBounds(x, y, w, h);
    dissonance_layout_.SetBounds(250, 0, 100, 152);
    filter_layout_.SetBounds(350, 0, 150, 156);
    effect_layout0_.SetBounds(0, 156, 150, 156);
    effect_layout1_.SetBounds(150, 156, 150, 156);
    effect_layout2_.SetBounds(300, 156, 150, 156);
    effect_layout3_.SetBounds(450, 156, 150, 156);
    effect_layout4_.SetBounds(600, 156, 150, 156);
    resynthsis_layout_.SetBounds(0, 156 * 2 + 30, 300, 70);
    standard_layout_.SetBounds(w - 100, y, 100, 70);
}
}