#pragma once

#include <memory>
#include "Parameter.h"

namespace mana {
class ParamCreator {
public:
    struct FloatCreateParam {
        ModulationType type;
        std::string id;
        std::string name;
        float vmin;
        float vmax;
        float vdefault;
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
};
}