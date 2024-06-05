#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <engine/modulation/curve_v2.h>

namespace mana::detail { // why did the pimpl broken on my computer?
class CurvePowerPointComponent;
class CurveXYPointComponent;

class CurveXYPointComponent
    : public juce::Component {
public:
    CurveXYPointComponent(std::shared_ptr<CurvePoint> p);

    void paint(juce::Graphics& g) override;
    void mouseDown(const juce::MouseEvent& e) override;
    void mouseDrag(const juce::MouseEvent& e) override;
    void mouseEnter(const juce::MouseEvent& e) override { repaint(); }
    void mouseExit(const juce::MouseEvent& e) override { repaint(); }
    void mouseDoubleClick(const juce::MouseEvent& e) override;

    // i don't know if use share_ptr will cause infinity reference?
    CurveXYPointComponent* before_{};
    CurveXYPointComponent* after_{};
    CurvePowerPointComponent* power_{};

    CurvePoint& GetPointRef() const {
        jassert(point_ != nullptr);
        return *point_;
    }

    std::shared_ptr<CurvePoint> GetPointPtr() const { return point_; }
    CurvePoint* GetRawPointPtr() const { return point_.get(); }
private:
    juce::ComponentDragger dragger_;
    std::shared_ptr<CurvePoint> point_;
};

class CurvePowerPointComponent
    : public juce::Component
    , public CurvePoint::Listener {
public:
    CurvePowerPointComponent();
    ~CurvePowerPointComponent() override;

    void paint(juce::Graphics& g) override;

    std::shared_ptr<CurvePoint> GetPointPtr() const {
        jassert(before_ != nullptr);
        return before_->GetPointPtr();
    }
    CurvePoint* GetRawPointPtr() const {
        jassert(before_ != nullptr);
        return before_->GetRawPointPtr();
    }
    CurvePoint& GetPointRef() const {
        // a power point should have a before xy point
        jassert(before_ != nullptr);
        return before_->GetPointRef();
    }

    void SetBeforeXyComp(CurveXYPointComponent* p);
    void SetAfterXyComp(CurveXYPointComponent* p);

    CurveXYPointComponent* GetRawBeforeXyPtr() const { return before_; }
    CurveXYPointComponent* GetRawAfterXyPtr() const { return after_; }
    CurveXYPointComponent& GetRawBeforeXy() const { return *before_; }
    CurveXYPointComponent& GetRawAfterXy() const { return *after_; }

    void mouseDrag(const juce::MouseEvent& e) override;
    void mouseEnter(const juce::MouseEvent& e) override { repaint(); }
    void mouseExit(const juce::MouseEvent& e) override { repaint(); }
private:
    CurveXYPointComponent* before_{};
    CurveXYPointComponent* after_{};

    // 通过 Listener 继承
    void OnChanged(CurvePoint* p, Property which) override;
};
}

namespace mana {
class CommonCurveEditor
    : public juce::Component
    , public CurveV2::Listener {
public:
    ~CommonCurveEditor() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

    void SetCurve(CurveV2* new_curve);
private:
    void mouseDoubleClick(const juce::MouseEvent& e) override;

    friend class ::mana::detail::CurveXYPointComponent;
    friend class ::mana::detail::CurvePowerPointComponent;

    void LimitXyPoint(detail::CurveXYPointComponent& p);
    void SetPowerPointPos(detail::CurvePowerPointComponent& p);
    void SetXyPointPos(detail::CurveXYPointComponent& p);
    juce::Rectangle<int> GetComponentBounds() const;

    CurveV2* curve_{};
    std::vector<std::unique_ptr<detail::CurveXYPointComponent>> xy_points_;
    std::vector<std::unique_ptr<detail::CurvePowerPointComponent>> power_points_;

    // 通过 Listener 继承
    void OnAddPoint(CurveV2* generator, std::shared_ptr<CurvePoint>, int before_idx) override;
    void OnRemovePoint(CurveV2* generator, std::shared_ptr<CurvePoint>) override;
    void OnReload(CurveV2* generator) override;
};
}