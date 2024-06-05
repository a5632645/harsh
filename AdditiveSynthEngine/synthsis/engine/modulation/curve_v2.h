#pragma once

#include <vector>
#include <memory>
#include <algorithm>
#include <cmath>
#include "utli/listener_list.h"

namespace mana {
class CurvePoint {
public:
    struct Listener {
        enum class Property {
            kX,
            kY,
            kPower
        };

        virtual ~Listener() = default;
        virtual void OnChanged(CurvePoint* p, Property which) = 0;
    };

    void AddListener(Listener* l) { listeners_.AddListener(l); }
    void RemoveListener(Listener* l) { listeners_.RemoveListener(l); }

    explicit CurvePoint(float x, float y, float p)
        : x_(x), y_(y), power_(p) {}

    void SetX(float x) {
        if (x != x_) {
            x_ = x;
            listeners_.CallListener(&Listener::OnChanged, this, Listener::Property::kX);
        }
    }

    void SetY(float y) {
        if (y != y_) {
            y_ = y;
            listeners_.CallListener(&Listener::OnChanged, this, Listener::Property::kY);
        }
    }

    void SetPower(float p) {
        if (p != power_) {
            power_ = p;
            listeners_.CallListener(&Listener::OnChanged, this, Listener::Property::kPower);
        }
    }

    float GetX() const { return x_; }
    float GetY() const { return y_; }
    float GetPower() const { return power_; }
private:
    utli::ListenerList<Listener> listeners_;

    float x_{};
    float y_{};
    float power_{};
};
}

namespace mana {
class CurveV2 {
public:
    static constexpr auto kLineResolution = 1024;
    static constexpr auto kLineSize = 1025;

    class Listener {
    public:
        virtual ~Listener() = default;
        virtual void OnAddPoint(CurveV2* generator, std::shared_ptr<CurvePoint>, int before_idx) = 0;
        virtual void OnRemovePoint(CurveV2* generator, std::shared_ptr<CurvePoint>) = 0;
        virtual void OnReload(CurveV2* generator) = 0;
    };

    CurveV2();

    void Remove(size_t idx);
    void Remove(std::shared_ptr<CurvePoint> point);
    void AddBehind(size_t idx, std::shared_ptr<CurvePoint> point);
    void AddPoint(std::shared_ptr<CurvePoint> point);

    CurvePoint& GetPointRef(int idx) { return *points_[idx]; }
    CurvePoint* GetRawPointPtr(int idx) { return points_[idx].get(); }
    std::shared_ptr<CurvePoint> GetPointPtr(int idx) { return points_[idx]; }

    decltype(auto) GetAllPoints() { return (points_); }
    decltype(auto) GetAllPoints() const { return (points_); }
    decltype(auto) GetDatas() const { return (datas_); }

    float Get(int idx) { return datas_[idx]; }
    float Get(float idx) {
        auto before = static_cast<int>(idx);
        auto after = before + 1;
        auto frac = idx - before;
        return std::lerp(Get(before), Get(after), frac);
    }
    float GetNormalize(float nor) { return Get((kLineResolution - 1) * nor); }

    int GetIndex(std::shared_ptr<CurvePoint> point) { return GetIndex(point.get()); }
    int GetIndex(CurvePoint* point) {
        for (int i = 0; const auto & p : points_) {
            if (p.get() == point)
                return i;
            ++i;
        }
        return -1;
    }
    void PartRender(int begin_point_idx, int end_point_idx);
    void FullRender() { PartRender(0, static_cast<int>(points_.size())); }

    void AddListener(Listener* l) { listeners_.AddListener(l); }
    void RemoveListener(Listener* l) { listeners_.RemoveListener(l); }
private:
    std::vector<std::shared_ptr<CurvePoint>> points_;
    std::vector<float> datas_;
    utli::ListenerList<Listener> listeners_;
};
}