#pragma once

#include <cassert>
#include <format>
#include <memory>
#include <string>
#include <tuple>
#include <string_view>
#include <unordered_map>
#include <nlohmann/json.hpp>
#include "param_range.h"
#include "Parameter.h"

namespace mana {
template<IsParamter... PT>
class TempParamBank {
public:
    template<IsParamter... Type> requires (sizeof...(Type) > 1)
        void AddParameter(std::unique_ptr<Type>... param) {
        (AddParameter<Type>(std::move(param)), ...);
    }

    template<IsParamter Type>
    void AddParameter(std::unique_ptr<Type> param) {
        auto& storer = GetParamMap<Type>();

        assert(param != nullptr);
        auto str_id = param->GetId();
        assert(!storer.contains(str_id));
        auto& v = *param;
        storer[str_id] = std::move(param);
    }

    template<IsParamter Type = FloatParameter, class... T> requires (sizeof...(T) >= 1)
        Type* GetParamPtr(std::format_string<T...> format_text, T&&...args) const {
        return GetParamPtr<Type>(std::format(format_text, std::forward<T>(args)...));
    }

    template<IsParamter Type = FloatParameter>
    Type* GetParamPtr(std::string_view id) const {
        auto& storer = GetParamMap<Type>();
        auto e = storer.at(id).get();
        assert(e != nullptr);
        return static_cast<Type*>(e);
    }

    template<IsParamter Type>
    using StoreType = std::unordered_map<std::string_view, std::unique_ptr<Type>>;

    template<IsParamter Type = FloatParameter>
    decltype(auto) GetParamMap() const {
        return std::get<StoreType<Type>>(parameters_);
    }

    template<IsParamter Type = FloatParameter>
    decltype(auto) GetParamMap() {
        return std::get<StoreType<Type>>(parameters_);
    }

    nlohmann::json SaveState() const {
        nlohmann::json outt;
        struct Saver {
            explicit Saver(nlohmann::json& out) : out(out) {}
            nlohmann::json& out;

            void operator()(const StoreType<FloatParameter>& s) {
                nlohmann::json tmp;
                std::ranges::for_each(s | std::views::values, [&tmp](const auto& ppram) {
                    tmp.push_back({
                        {"id", ppram->GetId()},
                        {"value", ppram->GetValue()}
                                  });
                });
                out["float"] = tmp;
            }
            void operator()(const StoreType<IntParameter>& s) {
                nlohmann::json tmp;
                std::ranges::for_each(s | std::views::values, [&tmp](const auto& ppram) {
                    tmp.push_back({
                        {"id", ppram->GetId()},
                        {"value", ppram->GetInt()}
                                  });
                });
                out["int"] = tmp;
            }
            void operator()(const StoreType<IntChoiceParameter>& s) {
                nlohmann::json tmp;
                std::ranges::for_each(s | std::views::values, [&tmp](const auto& ppram) {
                    tmp.push_back({
                        {"id", ppram->GetId()},
                        {"value", ppram->GetInt()}
                                  });
                });
                out["int_choice"] = tmp;
            }
            void operator()(const StoreType<BoolParameter>& s) {
                nlohmann::json tmp;
                std::ranges::for_each(s | std::views::values, [&tmp](const auto& ppram) {
                    tmp.push_back({
                        {"id", ppram->GetId()},
                        {"value", ppram->GetBool()}
                                  });
                });
                out["bool"] = tmp;
            }
        };
        std::apply([&outt](const auto&... params) {
            int _[]{ (std::invoke(Saver{ outt }, params), 0)... };
        }, parameters_);
        return outt;
    }

    void LoadState(const nlohmann::json& json) {
        struct Loader {
            explicit Loader(const nlohmann::json& json) : json(json) {}
            const nlohmann::json& json;

            void operator()(StoreType<FloatParameter>& storer) {
                auto t = json["float"];
                for (const auto& j : t) {
                    auto id = j["id"].get<std::string>();
                    auto val = j["value"].get<float>();
                    if (auto it = storer.find(id); it != storer.end())
                        it->second->SetValue(val);
                }
            }
            void operator()(StoreType<IntParameter>& storer) {
                auto t = json["int"];
                for (const auto& j : t) {
                    auto id = j["id"].get<std::string>();
                    auto val = j["value"].get<int>();
                    if (auto it = storer.find(id); it != storer.end())
                        it->second->SetValue(val);
                }
            }
            void operator()(StoreType<IntChoiceParameter>& storer) {
                auto t = json["int_choice"];
                for (const auto& j : t) {
                    auto id = j["id"].get<std::string>();
                    auto val = j["value"].get<int>();
                    if (auto it = storer.find(id); it != storer.end())
                        it->second->SetValue(val);
                }
            }
            void operator()(StoreType<BoolParameter>& storer) {
                auto t = json["bool"];
                for (const auto& j : t) {
                    auto id = j["id"].get<std::string>();
                    auto val = j["value"].get<bool>();
                    if (auto it = storer.find(id); it != storer.end())
                        it->second->SetValue(val);
                }
            }
        };

        std::apply([&json](auto&... params) {
            int _[]{ (std::invoke(Loader{ json }, params),0)... };
        }, parameters_);
    }
private:
    std::tuple<StoreType<PT>...> parameters_;
};

using ParamBank = TempParamBank<FloatParameter, IntParameter, IntChoiceParameter, BoolParameter>;
}