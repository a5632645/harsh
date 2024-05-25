#include <string_view>
#include "engine/modulation/Parameter.h"
#include "engine/modulation/param_creator.h"
#include "engine/synth.h"

using namespace mana;

class TestFloatParam : public mana::FloatParameter {
public:
    TestFloatParam(ParamCreator::FloatCreateParam p)
        : FloatParameter(p.type)
        , id(p.id) {}

    std::string id;

    // 通过 FloatParameter 继承
    void SetValue(float new_val) override
    {
    }
    float GetValue() const override
    {
        return 0.0f;
    }
    void Set01Value(float new_val) override
    {
    }
    float Get01Value() const override
    {
        return 0.0f;
    }
    float ConvertFrom01(float x) override
    {
        return 0.0f;
    }
    float ConvertTo01(float x) override
    {
        return 0.0f;
    }
    std::string_view GetId() const override
    {
        return id;
    }
    std::string_view GetName() const override
    {
        return std::string_view();
    }
    float PraseValue(std::string_view text) override
    {
        return 0.0f;
    }
    std::string FormantValue(float v) override
    {
        return std::string();
    }
};

class TestIntParam : public mana::IntParameter {
public:
    TestIntParam(ParamCreator::IntCreateParam p)
        : id(p.id) {}

    std::string id;

    // 通过 IntParameter 继承
    void SetValue(int new_val) override
    {
    }
    int GetInt() const override
    {
        return 0;
    }
    void Set01Value(float new_val) override
    {
    }
    float Get01Value() const override
    {
        return 0.0f;
    }
    int ConvertFrom01(float x) override
    {
        return 0;
    }
    float ConvertTo01(int x) override
    {
        return 0.0f;
    }
    std::string_view GetId() const override
    {
        return id;
    }
    std::string_view GetName() const override
    {
        return std::string_view();
    }
    int PraseValue(std::string_view text) override
    {
        return 0;
    }
    std::string FormantValue(int v) override
    {
        return std::string();
    }
};

class TestBoolParam : public mana::BoolParameter {
public:
    TestBoolParam(ParamCreator::BoolCreateParam p)
        : id(p.id) {}

    std::string id;

    // 通过 BoolParameter 继承
    void SetValue(bool new_val) override
    {
    }
    bool GetBool() const override
    {
        return false;
    }
    std::string_view GetId() const override
    {
        return id;
    }
    std::string_view GetName() const override
    {
        return std::string_view();
    }
    bool PraseValue(std::string_view text) override
    {
        return 0;
    }
    std::string FormantValue(bool v) override
    {
        return std::string();
    }
};

class TestEnumParam : public mana::IntChoiceParameter {
public:
    TestEnumParam(ParamCreator::IntChoiceCreateParam p)
        : id(p.id) {}

    std::string id;

    // 通过 IntChoiceParameter 继承
    void SetValue(int new_val) override
    {
    }
    int GetInt() const override
    {
        return 0;
    }
    void Set01Value(float new_val) override
    {
    }
    float Get01Value() const override
    {
        return 0.0f;
    }
    int ConvertFrom01(float x) override
    {
        return 0;
    }
    float ConvertTo01(int x) override
    {
        return 0.0f;
    }
    std::string_view GetId() const override
    {
        return id;
    }
    std::string_view GetName() const override
    {
        return std::string_view();
    }
    int PraseValue(std::string_view text) override
    {
        return 0;
    }
    std::string FormantValue(int v) override
    {
        return std::string();
    }
};

class TestCreator : public mana::ParamCreator {
public:
    // 通过 ParamCreator 继承
    std::unique_ptr<FloatParameter> CreateFloatParameter(FloatCreateParam p) override
    {
        return std::make_unique<TestFloatParam>(p);
    }
    std::unique_ptr<BoolParameter> CreateBoolParameter(BoolCreateParam p) override
    {
        return std::make_unique<TestBoolParam>(p);
    }
    std::unique_ptr<IntParameter> CreateIntParameter(IntCreateParam p) override
    {
        return std::make_unique<TestIntParam>(p);
    }
    std::unique_ptr<IntChoiceParameter> CreateIntChoiceParameter(IntChoiceCreateParam p) override
    {
        return std::make_unique<TestEnumParam>(p);
    }
};

int main() {
    Synth test_synth{ std::make_unique<TestCreator>() };
}