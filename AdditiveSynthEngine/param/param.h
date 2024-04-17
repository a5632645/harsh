#pragma once

#include <string>
#include <format>

namespace mana::param {
using namespace std::string_view_literals;

template<typename DetailParam>
    requires requires {
    DetailParam::kMin; DetailParam::kMax; DetailParam::kDefault; DetailParam::kTextPrecision;
}
struct FloatParam {
    static constexpr bool kHasStuff = requires {DetailParam::kStuff; };

    static constexpr float kNorDefault = (DetailParam::kDefault - DetailParam::kMin) / (DetailParam::kMax - DetailParam::kMin);

    static constexpr float GetNumber(float nor) {
        return std::lerp(DetailParam::kMin, DetailParam::kMax, nor);
    }

    static std::string GetText(float nor) {
        if constexpr (kHasStuff) {
            return std::format("{0:.{1}f} {2}", GetNumber(nor), DetailParam::kTextPrecision, DetailParam::kStuff);
        }
        else {
            return std::format("{0:.{1}f}", GetNumber(nor), DetailParam::kTextPrecision);
        }
    }
};

template<typename DetailParam, typename EnumType>
    requires requires {
    EnumType::kNumEnums; DetailParam::kNames;
}
struct FloatChoiceParam {
    static constexpr int kMaxIndex = static_cast<int>(EnumType::kNumEnums) - 1;

    static int GetChoiceIndex(float nor) {
        auto f = nor * kMaxIndex;
        return static_cast<int>(std::round(f));
        //return std::min(static_cast<int>(nor * static_cast<float>(EnumType::kNumEnums)), static_cast<int>(EnumType::kNumEnums) - 1);
    }

    static constexpr auto GetInterpIndex(float nor) {
        struct R {
            EnumType first;
            EnumType second;
            float frac;
        };

        auto f = nor * kMaxIndex;
        auto fd = static_cast<int>(f);
        auto fu = std::min(kMaxIndex, fd + 1);
        return R{ static_cast<EnumType>(fd), static_cast<EnumType>(fu), f - fd };
    }

    static constexpr EnumType GetEnum(float nor) {
        return static_cast<EnumType>(GetChoiceIndex(nor));
    }

    static constexpr std::string GetText(float nor) {
        return std::string{ DetailParam::kNames[GetChoiceIndex(nor)] };
    }
};

template<typename DetailParam, typename EnumType>
    requires requires {
    EnumType::kNumEnums; DetailParam::kNames;
}
struct IntChoiceParam {
    static constexpr int GetChoiceIndex(int x) {
        return std::min(x, static_cast<int>(EnumType::kNumEnums) - 1);
    }

    static constexpr EnumType GetEnum(int x) {
        return static_cast<EnumType>(GetChoiceIndex(x));
    }

    static constexpr std::string GetText(int x) {
        return DetailParam::kNames[GetChoiceIndex(x)];
    }
};
}