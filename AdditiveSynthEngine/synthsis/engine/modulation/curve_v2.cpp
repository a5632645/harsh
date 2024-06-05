#include "curve_v2.h"

#include <cassert>

namespace mana {
float CurveV2::GetPowerYValue(float nor_x, PowerEnum power_type, float power) {
    switch (power_type) {
    case PowerEnum::kKeep:
        return 0.0f;
    case PowerEnum::kPow:
    {
        constexpr auto max_pow = 10;
        if (power == 0.0f)
            return nor_x;
        else if (power < 0.0f) {
            auto mapped_power = std::lerp(1.0f, 10.0f, -power);
            auto pow_base = 1.0f / mapped_power;
            return std::pow(nor_x, pow_base);
        }
        else {
            auto mapped_power = std::lerp(1.0f, 10.0f, power);
            return std::pow(nor_x, mapped_power);
        }
    }
    default:
        assert(false);
        return 0.0f;
    }
}
}

namespace mana {
CurveV2::CurveV2() {
    datas_.resize(kLineSize);
    points_.clear();
    points_.emplace_back(0.0f, 0.0f);
    points_.emplace_back(1.0f, 1.0f);
    FullRender();
}

void CurveV2::Remove(int idx) {
    points_.erase(points_.cbegin() + idx);
    PartRender(idx - 1, idx + 1);
    listeners_.CallListener(&Listener::OnRemovePoint, this, idx);
}

void CurveV2::AddPoint(Point point) {
    const size_t num_loop = points_.size() - 1;
    for (size_t i = 0; i < num_loop; ++i) {
        if (point.x >= points_[i].x
            && point.x <= points_[i + 1].x) {
            AddBehind(i, point);
            return;
        }
    }
    AddBehind(points_.size() - 1, point);
}

void CurveV2::AddBehind(int idx, Point point) {
    points_.emplace(points_.begin() + idx + 1, point);
    PartRender(idx, idx + 2);
    listeners_.CallListener(&Listener::OnAddPoint, this, point, idx);
}

void CurveV2::PartRender(int begin_point_idx, int end_point_idx) {
    begin_point_idx = std::max(0, begin_point_idx);
    end_point_idx = std::min(end_point_idx, static_cast<int>(points_.size()));
    // i think add function will keep order
    // so we can use these to render
    // todo: thread safty
    for (int i = begin_point_idx; i < end_point_idx; ++i) {
        if (i + 1 >= static_cast<int>(points_.size()))
            break;

        const auto& curr_point = points_[i];
        const auto& next_point = points_[i + 1];
        auto begin_idx = static_cast<int>(std::round(curr_point.x * kLineResolution));
        auto end_idx = static_cast<int>(std::round(next_point.x * kLineResolution));

        if (begin_idx == end_idx) // do not render because it will divide by 0
            continue;

        auto curr_y = curr_point.y;
        auto next_y = next_point.y;
        auto x_range = end_idx - begin_idx;
        auto fx_range = static_cast<float>(x_range);
        for (int x = 0; x < x_range; ++x) {
            auto nor_x = x / fx_range;
            //datas_[x + begin_idx] = std::lerp(curr_y, next_y, nor_x);
            auto map_x = CurveV2::GetPowerYValue(nor_x, curr_point.power_type, curr_point.power);
            datas_[x + begin_idx] = std::lerp(curr_y, next_y, map_x);
        }
    }
    datas_[kLineResolution] = datas_[0];
}

void CurveV2::SetXy(int idx, float new_x, float new_y) {
    if (idx >= GetNumPoints())
        return;

    if (idx == 0)   // first
        new_x = 0.0f;
    else if (idx == GetNumPoints() - 1)
        new_x = 1.0f;
    else
        new_x = std::clamp(new_x, points_[idx - 1].x, points_[idx + 1].x);
    new_y = std::clamp(new_y, 0.0f, 1.0f);

    auto old_point = points_[idx];
    if (old_point.x != new_x || old_point.y != new_y) {
        old_point.x = new_x;
        old_point.y = new_y;
        points_[idx] = old_point;
        PartRender(idx - 1, idx + 1);
        listeners_.CallListener(&Listener::OnPointXyChanged, this, idx);
    }
}

void CurveV2::SetPower(int idx, float new_power) {
    if (idx >= GetNumPoints() - 1)
        return;

    new_power = std::clamp(new_power, -1.0f, 1.0f);

    auto old_power = points_[idx].power;
    if (old_power != new_power) {
        points_[idx].power = new_power;
        PartRender(idx, idx + 1);
        listeners_.CallListener(&Listener::OnPointPowerChanged, this, idx);
    }
}

void CurveV2::SetPowerType(int idx, PowerEnum new_type) {
    if (idx >= GetNumPoints() - 1)
        return;

    auto old_power = points_[idx].power_type;
    if (old_power != new_type) {
        points_[idx].power_type = new_type;
        PartRender(idx, idx + 1);
        listeners_.CallListener(&Listener::OnPointPowerChanged, this, idx);
    }
}
}