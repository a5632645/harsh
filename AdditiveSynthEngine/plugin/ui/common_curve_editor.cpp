#include "common_curve_editor.h"

static constexpr auto width = 20;
static constexpr auto height = width;
static constexpr auto kMaxOfDistance = 300;
static constexpr auto kCompCircleSize = 5.0f;

namespace mana::detail {
CurveXYPointComponent::CurveXYPointComponent(std::shared_ptr<CurvePoint> p)
    : point_(p) {
    setSize(width, height);
}

void CurveXYPointComponent::paint(juce::Graphics& g) {
    if (isMouseOver()) {
        g.setColour(juce::Colours::white);
        g.drawEllipse(getLocalBounds().toFloat(), 1.0f);
    }
    g.setColour(juce::Colours::white);
    g.fillEllipse(juce::Rectangle{ 0.0f,0.0f,kCompCircleSize,kCompCircleSize }.withCentre(getLocalBounds().getCentre().toFloat()));
}

void CurveXYPointComponent::mouseDown(const juce::MouseEvent& e) {
    dragger_.startDraggingComponent(this, e);
}

void CurveXYPointComponent::mouseDrag(const juce::MouseEvent& e) {
    dragger_.dragComponent(this, e, nullptr);
    auto& parent = *static_cast<CommonCurveEditor*>(getParentComponent());
    parent.LimitXyPoint(*this);

    auto center = getBounds().getCentre().toFloat();
    auto bound = parent.GetComponentBounds().toFloat();
    auto nor_x = (center.x - bound.getX()) / bound.getWidth();
    auto nor_y = 1.0f - (center.y - bound.getY()) / bound.getHeight();
    point_->SetX(nor_x);
    point_->SetY(nor_y);
    int changed_idx = parent.curve_->GetIndex(point_);
    parent.curve_->PartRender(changed_idx - 1, changed_idx + 1);
    parent.repaint();
}

void CurveXYPointComponent::mouseDoubleClick(const juce::MouseEvent& e) {
    if (point_ == nullptr)
        return;
    if (before_ == nullptr) // can not remove first point
        return;
    if (after_ == nullptr) // can not remove last point
        return;

    auto* pcurve = static_cast<CommonCurveEditor*>(getParentComponent())->curve_;
    if (pcurve != nullptr)
        pcurve->Remove(point_);
}

CurvePowerPointComponent::CurvePowerPointComponent() {
    setSize(width, height);
}

CurvePowerPointComponent::~CurvePowerPointComponent() {
    if (before_ != nullptr) {
        if (before_->GetPointPtr() != nullptr) {
            before_->GetPointRef().RemoveListener(this);
        }
    }
    if (after_ != nullptr) {
        if (after_->GetPointPtr() != nullptr) {
            after_->GetPointRef().RemoveListener(this);
        }
    }
}

void CurvePowerPointComponent::paint(juce::Graphics& g) {
    if (isMouseOver()) {
        g.setColour(juce::Colours::white);
        g.drawEllipse(getLocalBounds().toFloat(), 1.0f);
    }
    g.setColour(juce::Colours::white);
    g.drawEllipse(juce::Rectangle{ 0.0f,0.0f,kCompCircleSize,kCompCircleSize }.withCentre(getLocalBounds().getCentre().toFloat()), 1.0f);
}

void CurvePowerPointComponent::SetBeforeXyComp(CurveXYPointComponent* p) {
    jassert(p != nullptr);

    if (p == before_)
        return;

    if (before_ != nullptr && before_->GetPointPtr() != nullptr)
        before_->GetPointRef().RemoveListener(this);

    before_ = p;
    if (before_->GetPointPtr() != nullptr)
        before_->GetPointRef().AddListener(this);
}

void CurvePowerPointComponent::SetAfterXyComp(CurveXYPointComponent* p) {
    jassert(p != nullptr);

    if (p == after_)
        return;

    if (after_ != nullptr && after_->GetPointPtr() != nullptr)
        after_->GetPointRef().RemoveListener(this);

    after_ = p;
    if (after_->GetPointPtr() != nullptr)
        after_->GetPointRef().AddListener(this);
}

void CurvePowerPointComponent::mouseDrag(const juce::MouseEvent& e) {
    auto y = e.getDistanceFromDragStartY();
    auto nor_y = static_cast<float>(y) / static_cast<float>(kMaxOfDistance);
    nor_y = std::clamp(nor_y, -1.0f, 1.0f);
    GetPointRef().SetPower(nor_y);
}

void CurvePowerPointComponent::OnChanged(CurvePoint* p, Property which) {
    static_cast<CommonCurveEditor*>(getParentComponent())->SetPowerPointPos(*this);
}
}

namespace mana {
CommonCurveEditor::~CommonCurveEditor() {
    if (curve_ != nullptr)
        curve_->RemoveListener(this);
}

void CommonCurveEditor::paint(juce::Graphics& g) {
    g.setColour(juce::Colours::darkgrey);
    g.fillRect(GetComponentBounds());

    if (curve_ != nullptr) {
        const auto num_points = GetComponentBounds().getWidth();

        auto convert_xy = [bound = GetComponentBounds().toFloat()](float x, float y) {
            struct R {
                float x;
                float y;
            };
            return R{
                bound.getX() + bound.getWidth() * x,
                bound.getY() + bound.getHeight() * (1.0f - y)
            };
        };

        juce::Path p;
        p.preallocateSpace(num_points);
        {
            auto [x, y] = convert_xy(0, curve_->Get(0));
            p.startNewSubPath(x, y);
        }
        for (int i = 1; i < num_points; ++i) {
            auto nor_x = static_cast<float>(i) / static_cast<float>(num_points);
            auto nor_y = curve_->GetNormalize(nor_x);
            auto [x, y] = convert_xy(nor_x, nor_y);
            p.lineTo(x, y);
        }

        g.setColour(juce::Colours::green);
        g.strokePath(p, juce::PathStrokeType{ 1.0f });
    }

    g.setColour(juce::Colours::white);
    g.drawRect(GetComponentBounds());
}

void CommonCurveEditor::resized() {
    for (const auto& xy : xy_points_) {
        SetXyPointPos(*xy);
    }
    for (const auto& power : power_points_) {
        SetPowerPointPos(*power);
    }
}

void CommonCurveEditor::SetCurve(CurveV2* new_curve) {
    jassert(new_curve != nullptr);

    if (new_curve == curve_)
        return;

    if (curve_ != nullptr)
        curve_->RemoveListener(this);

    curve_ = new_curve;
    curve_->AddListener(this);
    OnReload(curve_);
}

void CommonCurveEditor::mouseDoubleClick(const juce::MouseEvent& e) {
    if (curve_ == nullptr)
        return;

    auto nor_x = (e.getMouseDownX() - GetComponentBounds().getX()) / static_cast<float>(GetComponentBounds().getWidth());
    auto nor_y = 1.0f - (e.getMouseDownY() - GetComponentBounds().getY()) / static_cast<float>(GetComponentBounds().getHeight());
    nor_x = std::clamp(nor_x, 0.0f, 1.0f);
    nor_y = std::clamp(nor_y, 0.0f, 1.0f);
    curve_->AddPoint(std::make_shared<CurvePoint>(nor_x, nor_y, 0.0f));
}

void CommonCurveEditor::LimitXyPoint(detail::CurveXYPointComponent& p) {
    auto left = GetComponentBounds().getX();
    auto right = GetComponentBounds().getTopRight().x;
    auto top = GetComponentBounds().getY();
    auto bottom = GetComponentBounds().getBottom();

    if (p.before_ == nullptr) { // first
        auto b = p.getBounds();
        auto center = b.getCentre();
        auto clamped_center = juce::Point{ left , std::clamp(center.y, top, bottom) };
        p.setBounds(b.withCentre(clamped_center));
    }
    else if (p.after_ == nullptr) { // last
        auto b = p.getBounds();
        auto center = b.getCentre();
        auto clamped_center = juce::Point{ right, std::clamp(center.y, top, bottom) };
        p.setBounds(b.withCentre(clamped_center));
    }
    else {
        auto b = p.getBounds();
        auto center = b.getCentre();
        auto clamped_center = juce::Point{ std::clamp(center.x, p.before_->getBounds().getCentreX(), p.after_->getBounds().getCentreX()),
            std::clamp(center.y, top, bottom) };
        p.setBounds(b.withCentre(clamped_center));
    }
}

void CommonCurveEditor::SetPowerPointPos(detail::CurvePowerPointComponent& p) {
    auto x = std::midpoint(p.GetRawBeforeXy().getBounds().getCentreX(),
                           p.GetRawAfterXy().getBounds().getCentreX());
    auto y = std::midpoint(p.GetRawBeforeXy().getBounds().getCentreY(),
                           p.GetRawAfterXy().getBounds().getCentreY());
    p.setBounds(p.getBounds().withCentre(juce::Point{ x,y }));
}

void CommonCurveEditor::SetXyPointPos(detail::CurveXYPointComponent& p) {
    const auto& point = p.GetPointRef();
    auto x = GetComponentBounds().getWidth() * point.GetX() + GetComponentBounds().getX();
    auto y = GetComponentBounds().getHeight() * (1.0f - point.GetY()) + GetComponentBounds().getY();
    p.setBounds(p.getBounds().toFloat().withCentre(juce::Point{ x,y }).toNearestInt());
}

juce::Rectangle<int> CommonCurveEditor::GetComponentBounds() const {
    return getLocalBounds().reduced(width / 2, height / 2);
}

void CommonCurveEditor::OnAddPoint(CurveV2* generator, std::shared_ptr<CurvePoint> point, int before_idx) {
    if (curve_ != generator)
        return;

    auto* before_point_rptr = curve_->GetRawPointPtr(before_idx);
    auto before_power_component_it = std::ranges::find_if(power_points_, [before_point_rptr](const auto& p) -> bool {
        return p->GetRawPointPtr() == before_point_rptr;
    });
    jassert(before_power_component_it != power_points_.cend());

    auto* before_xy = (*before_power_component_it)->GetRawBeforeXyPtr();
    auto* after_xy = (*before_power_component_it)->GetRawAfterXyPtr();
    auto* power_comp = (*before_power_component_it).get();
    jassert(before_xy != nullptr && after_xy != nullptr && power_comp != nullptr);

    auto& add_xy = xy_points_.emplace_back(std::make_unique<detail::CurveXYPointComponent>(point));
    auto& add_power = power_points_.emplace_back(std::make_unique<detail::CurvePowerPointComponent>());
    add_xy->before_ = before_xy;
    add_xy->after_ = after_xy;
    add_xy->power_ = add_power.get();
    before_xy->after_ = add_xy.get();
    after_xy->before_ = add_xy.get();
    power_comp->SetAfterXyComp(add_xy.get());
    add_power->SetBeforeXyComp(add_xy.get());
    add_power->SetAfterXyComp(after_xy);
    // todo: show new component and adjust addxy,before_power,add_power bounds
    addAndMakeVisible(add_xy.get());
    addAndMakeVisible(add_power.get());
    SetXyPointPos(*add_xy);
    LimitXyPoint(*add_xy);
    SetPowerPointPos(*add_power);
    SetPowerPointPos(*power_comp);

    curve_->PartRender(before_idx, before_idx + 2);
    repaint();
}

void CommonCurveEditor::OnRemovePoint(CurveV2* generator, std::shared_ptr<CurvePoint> point) {
    if (curve_ != generator)
        return;

    auto power_component_it = std::ranges::find_if(power_points_, [point_rptr = point.get()](const auto& p) -> bool {
        return p->GetRawPointPtr() == point_rptr;
    });
    jassert(power_component_it != power_points_.cend());

    auto* power_comp = (*power_component_it).get();
    auto* xy_comp = power_comp->GetRawBeforeXyPtr();
    jassert(power_comp != nullptr && xy_comp != nullptr);

    auto* before_xy = xy_comp->before_;
    auto before_power_it = std::ranges::find_if(power_points_, [before_xy](const auto& p) -> bool {
        return p->GetRawBeforeXyPtr() == before_xy;
    });
    auto* before_power = (*before_power_it).get();
    // because can not remove the first and the last point,these should not be nullptr
    jassert(before_xy != nullptr && before_power != nullptr && xy_comp->after_ != nullptr);

    before_xy->after_ = xy_comp->after_;
    xy_comp->after_->before_ = before_xy;
    before_power->SetAfterXyComp(xy_comp->after_);
    // todo: adjust before_power bounds
    SetPowerPointPos(*before_power);

    power_points_.erase(power_component_it);
    xy_points_.erase(std::ranges::find_if(xy_points_, [rptr = point.get()](const auto& p) -> bool {
        return p->GetRawPointPtr() == rptr;
    }));

    curve_->FullRender();
    repaint();
}

void CommonCurveEditor::OnReload(CurveV2* generator) {
    if (curve_ != generator)
        return;

    // rebuild interface
    power_points_.clear();
    xy_points_.clear();

    const auto& points = curve_->GetAllPoints();
    auto num_points = static_cast<int>(points.size());
    for (const auto& p : points) {
        xy_points_.emplace_back(std::make_unique<detail::CurveXYPointComponent>(p));
    }
    for (int i = 0; i < num_points - 1; ++i) {
        power_points_.emplace_back(std::make_unique<detail::CurvePowerPointComponent>());
    }
    // link xy points and power points
    xy_points_.front()->before_ = nullptr;
    xy_points_.front()->after_ = xy_points_[1].get();
    xy_points_.back()->after_ = nullptr;
    xy_points_.back()->before_ = xy_points_[num_points - 2].get();
    for (int i = 1; i < num_points - 1; ++i) {
        xy_points_[i]->before_ = xy_points_[i - 1].get();
        xy_points_[i]->after_ = xy_points_[i + 1].get();
        xy_points_[i]->power_ = power_points_[i].get();
    }
    for (int i = 0; i < num_points - 1; ++i) {
        power_points_[i]->SetBeforeXyComp(xy_points_[i].get());
        power_points_[i]->SetAfterXyComp(xy_points_[i + 1].get());
    }

    for (const auto& xy : xy_points_)
        addAndMakeVisible(xy.get());
    for (const auto& power : power_points_)
        addAndMakeVisible(power.get());

    resized();
    repaint();
}
}