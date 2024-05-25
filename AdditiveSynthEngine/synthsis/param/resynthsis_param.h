#include "param/param.h"

namespace mana::param {
struct Resynthsis_FreqScale : FloatParam<Resynthsis_FreqScale> {
    static constexpr int kArgIdx = 0;
    static constexpr auto kName = "f.scale"sv;
    static constexpr auto kMin = 0.0f;
    static constexpr auto kMax = 1.0f;
    static constexpr auto kDefault = 1.0f;
    static constexpr auto kTextPrecision = 2;
};

struct Resynthsis_FrameOffset : FloatParam<Resynthsis_FrameOffset> {
    static constexpr int kArgIdx = 1;
    static constexpr auto kName = "t.offset"sv;
    static constexpr auto kMin = 0.0f;
    static constexpr auto kMax = 1.0f;
    static constexpr auto kDefault = 0.0f;
    static constexpr auto kTextPrecision = 5;
};

struct Resynthsis_FrameSpeed : FloatParam<Resynthsis_FrameSpeed> {
    static constexpr int kArgIdx = 2;
    static constexpr auto kName = "speed"sv;
    static constexpr auto kMin = -3.0f;
    static constexpr auto kMax = 3.0f;
    static constexpr auto kDefault = 1.0f;
    static constexpr auto kTextPrecision = 3;
};

struct Resynthsis_FormantMix : FloatParam<Resynthsis_FormantMix> {
    static constexpr int kArgIdx = 3;
    static constexpr auto kName = "fm.mix"sv;
    static constexpr auto kMin = 0.0f;
    static constexpr auto kMax = 1.0f;
    static constexpr auto kDefault = 0.0f;
    static constexpr auto kTextPrecision = 2;
};

struct Resynthsis_FormantShift : FloatParam<Resynthsis_FormantShift> {
    static constexpr int kArgIdx = 4;
    static constexpr auto kName = "fm.shift"sv;
    static constexpr auto kMin = -12.0f;
    static constexpr auto kMax = 12.0f;
    static constexpr auto kDefault = 0.0f;
    static constexpr auto kTextPrecision = 1;
    static constexpr auto kStuff = "st";
};

struct Resynthsis_GainMix : FloatParam<Resynthsis_GainMix> {
    static constexpr int kArgIdx = 5;
    static constexpr auto kName = "g.mix"sv;
    static constexpr auto kMin = 0.0f;
    static constexpr auto kMax = 1.0f;
    static constexpr auto kDefault = 1.0f;
    static constexpr auto kTextPrecision = 2;
};
}