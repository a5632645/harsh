#pragma once

#include <string>
#include <format>
#include <string_view>
#include <array>
#include "engine/poly_param.h"
#include "engine/modulation/param_range.h"
#include "engine/modulation/Parameter.h"

namespace mana::param {
using namespace std::string_view_literals;

template<typename DetailParam, float(*remap_func)(float) = nullptr>
struct FloatParam {
    template<typename P = DetailParam> requires requires{
        P::kMin;
        P::kMax;
        P::kDefault;
    }
    static constexpr ParamRange CreateRange() {
        return ParamRange{ P::kMin, P::kMax, P::kDefault };
    }

    template<typename P = DetailParam> requires requires {
        P::kId;
    }
    static constexpr std::unique_ptr<FloatParameter> CreateParam(ModulationType type) {
        return CreateParam(type, P::kId);
    }

    template<typename P = DetailParam, class... T> requires requires {
        P::kName;
    }
    static constexpr std::unique_ptr<FloatParameter> CreateParam(ModulationType type,
                                                                 std::format_string<T...> id_format_text,
                                                                 T&&... id_args) {
        auto p = std::make_unique<FloatParameter>(type,
                                                  CreateRange(),
                                                  P::kName,
                                                  id_format_text, std::forward<T>(id_args)...);
        p->value_to_text = GetTextValue;
        return p;
    }

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

        if constexpr (remap_func != nullptr) {
            nor = remap_func(nor);
        }

        return std::lerp(P::kMin, P::kMax, nor);
    }

    template<typename P = DetailParam> requires requires {
        P::kMin;
        P::kMax;
    }
    static constexpr float GetNumber(PolyModuFloatParameter& nor) {
        static_assert(requires {P::kMin; P::kMax; });

        auto v = nor.GetValue();
        if constexpr (remap_func != nullptr) {
            v = remap_func(v);
        }

        return std::lerp(P::kMin, P::kMax, v);
    }

    template<size_t N, typename P = DetailParam> requires requires {
        P::kArgIdx;
        P::kArgIdx < N;
    }
    static constexpr float GetNumber(const std::array<float, N>& arr) {
        return GetNumber(arr[P::kArgIdx]);
    }

    template<size_t N, typename P = DetailParam> requires requires {
        P::kArgIdx;
        P::kArgIdx < N;
    }
    static constexpr float GetNumber(const std::array<PolyModuFloatParameter*, N>& arr) {
        return GetNumber(arr[P::kArgIdx]->GetValue());
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

    static std::string GetTextValue(float v) {
        constexpr bool kHasStuff = requires {DetailParam::kStuff; DetailParam::kTextPrecision; };

        if constexpr (kHasStuff) {
            return std::format("{0:.{1}f}{2}", v, DetailParam::kTextPrecision, DetailParam::kStuff);
        }
        else {
            return std::format("{0:.{1}f}", v, DetailParam::kTextPrecision);
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

    template<std::ranges::input_range RNG> requires std::same_as<std::ranges::range_value_t<RNG>, PolyModuFloatParameter*>
    static constexpr auto GetChoiceIndex(RNG&& args) {
        static_assert(requires{DetailParam::kArgIdx; });
        return GetChoiceIndex(args[DetailParam::kArgIdx]->GetValue());
    }

    static constexpr auto GetInterpIndex(float nor) {
        using EnumType = typename DetailParam::ParamEnum;

        auto f = nor * GetMaxChoiceIndex();
        auto fd = static_cast<int>(f);
        auto fu = std::min(GetMaxChoiceIndex(), fd + 1);
        return std::tuple{ static_cast<EnumType>(fd), static_cast<EnumType>(fu), f - fd };
    }

    static constexpr auto GetInterpIndex(PolyModuFloatParameter& nor) {
        return GetInterpIndex(nor.GetValue());
    }

    template<std::ranges::input_range RNG>  requires std::same_as<std::ranges::range_value_t<RNG>, float>
    static constexpr auto GetInterpIndex(RNG&& args) {
        static_assert(requires{DetailParam::kArgIdx; });
        return GetInterpIndex(args[DetailParam::kArgIdx]);
    }

    template<std::ranges::input_range RNG> requires std::same_as<std::ranges::range_value_t<RNG>, PolyModuFloatParameter*>
    static constexpr auto GetInterpIndex(RNG&& args) {
        static_assert(requires{DetailParam::kArgIdx; });
        return GetInterpIndex(args[DetailParam::kArgIdx]->GetValue());
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
    template<typename P = DetailParam> requires requires{
        P::kNames;
    }
    static constexpr ParamRange CreateRange() {
        return ParamRange{ 0.0f, static_cast<float>(std::size(P::kNames)) };
    }

    template<typename P = DetailParam> requires requires {
        P::kId;
    }
    static constexpr std::unique_ptr<IntChoiceParameter> CreateParam(ModulationType type) {
        return CreateParam(type, P::kId);
    }

    template<typename P = DetailParam, class... T> requires requires {
        P::kName;
        P::kNames;
    }
    static constexpr std::unique_ptr<IntChoiceParameter> CreateParam(ModulationType type,
                                                                     std::format_string<T...> id_format_text,
                                                                     T&&... id_args) {
        auto p = std::make_unique<IntChoiceParameter>(type,
                                                      CreateRange(),
                                                      P::kName,
                                                      id_format_text, std::forward<T>(id_args)...);
        p->choices_ = std::vector(std::begin(P::kNames), std::end(P::kNames));
        p->value_to_text = GetTextFromFloat;
        return p;
    }

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

        return std::string{ DetailParam::kNames[GetChoiceIndex(x)] };
    }

    static std::string GetTextFromFloat(float x) {
        return GetText(std::round(x));
    }
};
}