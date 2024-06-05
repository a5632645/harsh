#pragma once

#include <vector>
#include <memory>
#include <algorithm>
#include <cmath>
#include "utli/listener_list.h"

namespace mana {
class CurveV2 {
public:
    static constexpr auto kLineResolution = 1024;
    static constexpr auto kLineSize = 1025;

    enum class PowerEnum {
        kKeep = 0,
        kPow,
        kNumPowerEnums
    };
    static float GetPowerYValue(float nor_x, PowerEnum power_type, float power);

    struct Point {
        constexpr Point(float x, float y, float power = 0.0f)
            : x(x), y(y), power(power) {}

        float x;
        float y;
        float power;
        PowerEnum power_type{ PowerEnum::kPow };
    };

    class Listener {
    public:
        virtual ~Listener() = default;
        virtual void OnAddPoint(CurveV2* generator, Point p, int before_idx) = 0;
        virtual void OnRemovePoint(CurveV2* generator, int remove_idx) = 0;
        virtual void OnPointXyChanged(CurveV2* generator, int changed_idx) = 0;
        virtual void OnPointPowerChanged(CurveV2* generator, int changed_idx) = 0;
        virtual void OnReload(CurveV2* generator) = 0;
    };

    CurveV2();

    void Remove(int idx);
    void AddBehind(int idx, Point point);
    void AddPoint(Point point);

    decltype(auto) GetDatas() const { return (datas_); }
    float Get(int idx) { return datas_[idx]; }
    float Get(float idx) {
        auto before = static_cast<int>(idx);
        auto after = before + 1;
        auto frac = idx - before;
        return std::lerp(Get(before), Get(after), frac);
    }
    float GetNormalize(float nor) { return Get((kLineResolution - 1) * nor); }

    decltype(auto) GetAllPoints() { return (points_); }
    decltype(auto) GetAllPoints() const { return (points_); }
    int GetNumPoints() const { return static_cast<int>(points_.size()); }
    Point GetPoint(int idx) { return points_[idx]; }

    void SetXy(int idx, float new_x, float new_y);
    void SetPower(int idx, float new_power);
    void SetPowerType(int idx, PowerEnum new_type);

    void PartRender(int begin_point_idx, int end_point_idx);
    void FullRender() { PartRender(0, static_cast<int>(points_.size())); }

    void AddListener(Listener* l) { listeners_.AddListener(l); }
    void RemoveListener(Listener* l) { listeners_.RemoveListener(l); }
private:
    std::vector<Point> points_;
    std::vector<float> datas_;
    utli::ListenerList<Listener> listeners_;
};
}