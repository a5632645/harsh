#pragma once

#include <string>
#include <format>
#include <string_view>
#include <array>
#include <tuple>
#include "engine/modulation/modu_type.h"

namespace mana::param {
using namespace std::string_view_literals;
using namespace std::string_literals;

namespace param_tag {
struct FloatTag {};
struct IntTag {};
struct FloatChoiceTag {};
struct IntChoiceTag {};
}

template<typename DetailParam>
struct FloatParam {
    using Tag = param_tag::FloatTag;

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
    static constexpr float ConvertFrom01(float val01) {
        return std::lerp(P::kMin, P::kMax, val01);
    }

    template<typename P = DetailParam> requires requires {
        P::kMin;
        P::kMax;
    }
    static constexpr float ConvertTo01(float val) {
        return (val - P::kMin) / (P::kMax - P::kMin);
    }

    static std::string GetText(float num) {
        if constexpr (requires{DetailParam::kStuff; }) {
            return std::format("{0:.{1}f}{2}", num, DetailParam::kTextPrecision, DetailParam::kStuff);
        }
        else {
            return std::format("{0:.{1}f}", num, DetailParam::kTextPrecision);
        }
    }

    static std::string GetTextFrom01(float val01) {
        return GetText(ConvertFrom01(val01));
    }

    static constexpr float GetDefault() {
        if constexpr (requires{DetailParam::kDefault; }) {
            return DetailParam::kDefault;
        }
        else {
            return DetailParam::kMin;
        }
    }

    static constexpr ModulationType GetModuType() {
        if constexpr (requires{DetailParam::kModuType; }) {
            return DetailParam::kModuType;
        }
        else {
            return ModulationType::kModulable;
        }
    }

    static constexpr float GetBlend() {
        if constexpr (requires{DetailParam::kBlend; }) {
            return DetailParam::kBlend;
        }
        else {
            return 1.0f;
        }
    }
};

template<typename P>
struct FloatTimeParam : FloatParam<P> {
    static constexpr auto kBlend = 0.3f;
};

template<typename DetailParam>
struct FloatChoiceParam {
    using Tag = param_tag::FloatChoiceTag;

    static constexpr float GetNormalDefault() {
        if constexpr (requires{DetailParam::kDefault; }) {
            return DetailParam::kDefault;
        }
        else {
            return 0.0f;
        }
    }

    static constexpr int GetNumChoices() {
        if constexpr (std::is_enum_v<typename DetailParam::ParamEnum>) {
            static_assert(static_cast<std::size_t>(DetailParam::ParamEnum::kNumEnums) == std::size(DetailParam::kNames),
                          "enum and names count mismatch");
            using EnumType = typename DetailParam::ParamEnum;
            return static_cast<int>(EnumType::kNumEnums);
        }
        else {
            return static_cast<int>(std::size(DetailParam::kNames));
        }
    }

    static constexpr int GetMaxChoiceIndex() {
        return GetNumChoices() - 1;
    }

    static int GetChoiceIndexFrom01(float val01) {
        return static_cast<int>(val01 * (GetNumChoices() - 0.1f));
    }

    template<typename P = DetailParam> requires std::is_enum_v<typename P::ParamEnum>
    static constexpr auto GetInterpIndexFrom01(float val01) {
        using EnumType = typename P::ParamEnum;
        auto f = val01 * GetMaxChoiceIndex();
        auto fd = static_cast<int>(f);
        auto fu = std::min(GetMaxChoiceIndex(), fd + 1);
        return std::tuple{ static_cast<EnumType>(fd), static_cast<EnumType>(fu), f - fd };
    }

    template<typename P = DetailParam> requires std::is_enum_v<typename P::ParamEnum>
    static constexpr auto GetEnumFrom01(float val01) {
        return static_cast<typename DetailParam::ParamEnum>(GetChoiceIndexFrom01(val01));
    }

    template<typename P = DetailParam> requires std::is_enum_v<typename P::ParamEnum>
    static constexpr auto GetEnum(int val) {
        return static_cast<typename DetailParam::ParamEnum>(val);
    }

    template<typename P = DetailParam> requires requires {
        P::kNames;
    }
    static constexpr std::string GetText(int val) {
        return std::string{ DetailParam::kNames[val] };
    }

    static constexpr std::string GetTextFrom01(float val01) {
        return GetText(GetChoiceIndexFrom01(val01));
    }

    static constexpr ModulationType GetModuType() {
        if constexpr (requires{DetailParam::kModuType; }) {
            return DetailParam::kModuType;
        }
        else {
            return ModulationType::kModulable;
        }
    }
};

template<typename DetailParam>
struct IntChoiceParam {
    using Tag = param_tag::IntChoiceTag;

    static constexpr int GetDefault() {
        if constexpr (requires{DetailParam::kDefault; }) {
            return DetailParam::kDefault;
        }
        else {
            return 0;
        }
    }

    template<typename P = DetailParam> requires std::is_enum_v<typename P::ParamEnum>
    static constexpr auto GetEnum(int x) {
        using EnumType = typename P::ParamEnum;
        return static_cast<EnumType>(x);
    }
};

template<typename DetailParam>
struct IntParam {
    using Tag = param_tag::IntTag;

    static constexpr int GetDefault() {
        if constexpr (requires{DetailParam::kDefault; }) {
            return DetailParam::kDefault;
        }
        else {
            return DetailParam::kMin;
        }
    }

    static constexpr std::string GetText(int x) {
        return std::to_string(x);
    }

    static std::string GetTextFromFloat(float x) {
        return GetText(std::round(x));
    }
};

template<typename P>
using ParamTag_T = typename P::Tag;
}