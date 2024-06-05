#include "curve_v2.h"

namespace mana {
CurveV2::CurveV2() {
    datas_.resize(kLineSize);
    points_.clear();
    points_.emplace_back(std::make_shared<CurvePoint>(0.0f, 0.0f, 0.0f));
    points_.emplace_back(std::make_shared<CurvePoint>(1.0f, 1.0f, 0.0f));
    FullRender();
}

void CurveV2::Remove(size_t idx) {
    Remove(points_.at(idx));
}

void CurveV2::Remove(std::shared_ptr<CurvePoint> point) {
    auto it = std::ranges::find(points_, point);
    if (it == points_.cend())
        return;

    points_.erase(it);
    listeners_.CallListener(&Listener::OnRemovePoint, this, point);
}

void CurveV2::AddPoint(std::shared_ptr<CurvePoint> point) {
    const size_t num_loop = points_.size() - 1;
    for (size_t i = 0; i < num_loop; ++i) {
        if (point->GetX() >= points_[i]->GetX()
            && point->GetX() <= points_[i + 1]->GetX()) {
            AddBehind(i, point);
            return;
        }
    }
    AddBehind(points_.size() - 1, point);
}

void CurveV2::AddBehind(size_t idx, std::shared_ptr<CurvePoint> point) {
    points_.emplace(points_.begin() + idx + 1, point);
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

        const auto& curr_point = *points_[i];
        const auto& next_point = *points_[i + 1];
        auto begin_idx = static_cast<int>(std::round(curr_point.GetX() * kLineResolution));
        auto end_idx = static_cast<int>(std::round(next_point.GetX() * kLineResolution));

        if (begin_idx == end_idx) // do not render because it will divide by 0
            continue;

        auto line_power = curr_point.GetPower();
        auto curr_y = curr_point.GetY();
        auto next_y = next_point.GetY();
        auto x_range = end_idx - begin_idx;
        auto fx_range = static_cast<float>(x_range);
        for (int x = 0; x < x_range; ++x) {
            auto nor_x = x / fx_range;
            datas_[x + begin_idx] = std::lerp(curr_y, next_y, nor_x);
        }
    }
    datas_[kLineResolution] = datas_[0];
}
}