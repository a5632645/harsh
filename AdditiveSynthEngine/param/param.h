#pragma once

#include <string>
#include <format>
#include <functional>
#include "engine/oscillor_param.h"

namespace mana::param {
using namespace std::string_view_literals;

template<typename DetailParam, float(*remap_func)(float) = nullptr>
struct FloatParam {
    static constexpr float GetNormalDefault() {
        static_assert(requires {DetailParam::kMin; DetailParam::kMax; DetailParam::kDefault; });
        return (DetailParam::kDefault - DetailParam::kMin) / (DetailParam::kMax - DetailParam::kMin);
    }

    static constexpr float GetNumber(float nor) {
        static_assert(requires {DetailParam::kMin; DetailParam::kMax; });

        if constexpr (remap_func != nullptr) {
            nor = remap_func(nor);
        }

        return std::lerp(DetailParam::kMin, DetailParam::kMax, nor);
    }

    static constexpr float GetNumber(SingleOscillorParam nor) {
        static_assert(requires {DetailParam::kMin; DetailParam::kMax; });

        auto v = nor.GetClamp();
        if constexpr (remap_func != nullptr) {
            v = remap_func(v);
        }

        return std::lerp(DetailParam::kMin, DetailParam::kMax, v);
    }

    template<size_t N>
    static constexpr float GetNumber(const std::array<float, N>& arr) {
        constexpr bool kHasArgIdx = requires {DetailParam::kArgIdx; };
        static_assert(kHasArgIdx, "must have an arg index.");
        static_assert(DetailParam::kArgIdx < N, "arg idx out of bounds.");

        return GetNumber(arr[DetailParam::kArgIdx]);
    }

    template<size_t N>
    static constexpr float GetNumber(const std::array<SingleOscillorParam, N>& arr) {
        constexpr bool kHasArgIdx = requires {DetailParam::kArgIdx; };
        static_assert(kHasArgIdx, "must have an arg index.");
        static_assert(DetailParam::kArgIdx < N, "arg idx out of bounds.");

        return GetNumber(arr[DetailParam::kArgIdx].GetClamp());
    }

    static std::string GetText(float nor) {
        constexpr bool kHasStuff = requires {DetailParam::kStuff; DetailParam::kTextPrecision; };

        if constexpr (kHasStuff) {
            return std::format("{0:.{1}f}{2}", GetNumber(nor), DetailParam::kTextPrecision, DetailParam::kStuff);
        }
        else {
            return std::format("{0:.{1}f}", GetNumber(nor), DetailParam::kTextPrecision);
        }
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

    template<std::ranges::input_range RNG> requires std::same_as<std::ranges::range_value_t<RNG>, float>
    static constexpr auto GetChoiceIndex(RNG&& args) {
        static_assert(requires{DetailParam::kArgIdx; });
        return GetChoiceIndex(args[DetailParam::kArgIdx]);
    }

    template<std::ranges::input_range RNG> requires std::same_as<std::ranges::range_value_t<RNG>, SingleOscillorParam>
    static constexpr auto GetChoiceIndex(RNG&& args) {
        static_assert(requires{DetailParam::kArgIdx; });
        return GetChoiceIndex(args[DetailParam::kArgIdx].GetClamp());
    }

    static constexpr auto GetInterpIndex(float nor) {
        using EnumType = typename DetailParam::ParamEnum;
        struct R {
            EnumType first;
            EnumType second;
            float frac;
        };

        auto f = nor * GetMaxChoiceIndex();
        auto fd = static_cast<int>(f);
        auto fu = std::min(GetMaxChoiceIndex(), fd + 1);
        return R{ static_cast<EnumType>(fd), static_cast<EnumType>(fu), f - fd };
    }

    static constexpr auto GetInterpIndex(SingleOscillorParam nor) {
        using EnumType = typename DetailParam::ParamEnum;
        struct R {
            EnumType first;
            EnumType second;
            float frac;
        };

        auto f = nor.GetClamp() * GetMaxChoiceIndex();
        auto fd = static_cast<int>(f);
        auto fu = std::min(GetMaxChoiceIndex(), fd + 1);
        return R{ static_cast<EnumType>(fd), static_cast<EnumType>(fu), f - fd };
    }

    template<std::ranges::input_range RNG>  requires std::same_as<std::ranges::range_value_t<RNG>, float>
    static constexpr auto GetInterpIndex(RNG&& args) {
        static_assert(requires{DetailParam::kArgIdx; });
        return GetInterpIndex(args[DetailParam::kArgIdx]);
    }

    template<std::ranges::input_range RNG> requires std::same_as<std::ranges::range_value_t<RNG>, SingleOscillorParam>
    static constexpr auto GetInterpIndex(RNG&& args) {
        static_assert(requires{DetailParam::kArgIdx; });
        return GetInterpIndex(args[DetailParam::kArgIdx].GetClamp());
    }

    static constexpr auto GetEnum(float nor) {
        using EnumType = typename DetailParam::ParamEnum;

        return static_cast<EnumType>(GetChoiceIndex(nor));
    }

    static constexpr std::string GetText(float nor) {
        using EnumType = typename DetailParam::ParamEnum;

        static_assert(requires{DetailParam::kNames; });
        static_assert(static_cast<size_t>(EnumType::kNumEnums) == std::size(DetailParam::kNames));

        return std::string{ DetailParam::kNames[GetChoiceIndex(nor)] };
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

    static constexpr std::string GetText(int x) {
        using EnumType = typename DetailParam::ParamEnum;
        static_assert(requires{DetailParam::kNames; });
        static_assert(static_cast<size_t>(EnumType::kNumEnums) == std::size(DetailParam::kNames));

        return DetailParam::kNames[GetChoiceIndex(x)];
    }
};
}