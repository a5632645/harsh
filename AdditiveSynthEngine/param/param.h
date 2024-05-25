#pragma once

#include <string>
#include <format>
#include <string_view>
#include <array>

namespace mana::param {
using namespace std::string_view_literals;

template<typename DetailParam>
struct FloatParam {
    template<typename P = DetailParam> requires requires {
        P::kMin;
        P::kMax;
        P::kDefault;
    }
    static constexpr float GetNormalDefault() {
        return (P::kDefault - P::kMin) / (P::kMax - P::kMin);
    }

    template<typename P = DetailParam> requires requires {
        P::kMin;
        P::kMax;
    }
    static constexpr float GetNumber(float nor) {
        static_assert(requires {P::kMin; P::kMax; });
        return std::lerp(P::kMin, P::kMax, nor);
    }
};

template<typename DetailParam>
struct FloatChoiceParam {
    static constexpr float GetNormalDefault() {
        return 0.0f;
    }

    static constexpr int GetMaxChoiceIndex() {
        using EnumType = typename DetailParam::ParamEnum;

        static_assert(requires{EnumType::kNumEnums; });
        return static_cast<int>(EnumType::kNumEnums) - 1;;
    }

    static int GetChoiceIndex(float nor) {
        auto f = nor * GetMaxChoiceIndex();
        return static_cast<int>(std::round(f));
    }

    static constexpr auto GetInterpIndex(float nor) {
        using EnumType = typename DetailParam::ParamEnum;

        auto f = nor * GetMaxChoiceIndex();
        auto fd = static_cast<int>(f);
        auto fu = std::min(GetMaxChoiceIndex(), fd + 1);
        return std::tuple{ static_cast<EnumType>(fd), static_cast<EnumType>(fu), f - fd };
    }

    static constexpr auto GetEnum(float nor) {
        using EnumType = typename DetailParam::ParamEnum;

        return static_cast<EnumType>(GetChoiceIndex(nor));
    }

    static constexpr auto GetEnum(int nor) {
        using EnumType = typename DetailParam::ParamEnum;

        return static_cast<EnumType>(nor);
    }
};

template<typename DetailParam>
struct IntChoiceParam {
    static constexpr int GetChoiceIndex(int x) {
        using EnumType = typename DetailParam::ParamEnum;
        static_assert(requires{EnumType::kNumEnums; });

        return std::min(x, static_cast<int>(EnumType::kNumEnums) - 1);
    }

    static constexpr auto GetEnum(int x) {
        using EnumType = typename DetailParam::ParamEnum;

        return static_cast<EnumType>(GetChoiceIndex(x));
    }
};

template<typename DetailParam>
struct IntParam {
    static constexpr std::string GetText(int x) {
        return std::to_string(x);
    }

    static std::string GetTextFromFloat(float x) {
        return GetText(std::round(x));
    }
};
}