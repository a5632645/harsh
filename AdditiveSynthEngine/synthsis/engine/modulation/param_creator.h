#pragma once

#include <memory>
#include "Parameter.h"
#include "param/param.h"

namespace mana {
class ParamCreator {
public:
    struct FloatCreateParam {
        ModulationType type;
        std::string id;
        std::string name;
        float vmin{};
        float vmax{};
        float vdefault{};
        float vblend{ 1.0f };
    };

    struct IntCreateParam {
        std::string id;
        std::string name;
        int vmin;
        int vmax;
        int vdefault;
    };

    struct BoolCreateParam {
        std::string id;
        std::string name;
        bool vdefault;
    };

    struct IntChoiceCreateParam {
        std::string id;
        std::string name;
        std::vector<std::string_view> choices;
        int vdefault;
    };

    virtual ~ParamCreator() = default;

    virtual std::unique_ptr<FloatParameter> CreateFloatParameter(FloatCreateParam p) = 0;
    virtual std::unique_ptr<BoolParameter> CreateBoolParameter(BoolCreateParam p) = 0;
    virtual std::unique_ptr<IntParameter> CreateIntParameter(IntCreateParam p) = 0;
    virtual std::unique_ptr<IntChoiceParameter> CreateIntChoiceParameter(IntChoiceCreateParam p) = 0;

    template<typename P> requires requires {
        P::kId;
    }
    auto CreateParameter(P p) {
        return CreateParameterWithId(p, std::string{ P::kId });
    }

    template<typename P, class... Args> requires requires {
        P::kIdFormater;
    }
    auto CreateParameter(P p, Args&&... id_args) {
        return CreateParameterWithId(p, std::format(P::kIdFormater, std::forward<Args>(id_args)...));
    }

    auto CreateUnitParameter(ModulationType type, std::string name, std::string id) {
        FloatCreateParam c;
        c.id = std::move(id);
        c.name = std::move(name);
        c.type = type;
        c.vblend = 1.0f;
        c.vdefault = 0.0f;
        c.vmax = 1.0f;
        c.vmin = 0.0f;
        return CreateFloatParameter(std::move(c));
    }
private:
    template<typename P> requires std::same_as<typename P::Tag, param::param_tag::FloatTag>
    auto CreateParameterWithId(P, std::string id) {
        FloatCreateParam c;
        c.id = std::move(id);
        c.name = std::string{ P::kName };
        c.type = P::GetModuType();
        c.vmin = P::kMin;
        c.vmax = P::kMax;
        c.vdefault = P::GetDefault();
        c.vblend = P::GetBlend();
        return CreateFloatParameter(std::move(c));
    }
    template<typename P> requires std::same_as<typename P::Tag, param::param_tag::FloatChoiceTag>
    auto CreateParameterWithId(P, std::string id) {
        return CreateUnitParameter(P::GetModuType(), std::string{P::kName}, std::move(id));
    }
    template<typename P> requires std::same_as<typename P::Tag, param::param_tag::IntChoiceTag>
    auto CreateParameterWithId(P, std::string id) {
        IntChoiceCreateParam c;
        c.id = std::move(id);
        c.name = std::string{ P::kName };
        c.vdefault = P::GetDefault();
        c.choices = { std::cbegin(P::kNames), std::cend(P::kNames) };
        return CreateIntChoiceParameter(std::move(c));
    }
    template<typename P> requires std::same_as<typename P::Tag, param::param_tag::IntTag>
    auto CreateParameterWithId(P, std::string id) {
        IntCreateParam c;
        c.id = std::move(id);
        c.name = std::string{ P::kName };
        c.vmin = P::kMin;
        c.vmax = P::kMax;
        c.vdefault = P::GetDefault();
        return CreateIntParameter(std::move(c));
    }
};
}