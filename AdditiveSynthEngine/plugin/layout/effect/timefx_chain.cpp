#include "timefx_chain.h"

#include <engine/synth.h>
#include "delay_section.h"
#include "distortion_section.h"
#include "chrous_section.h"
#include "reverb_section.h"

namespace mana {
TimeFxChain::TimeFxChain(Synth& synth)
    : fxchain_(synth.GetTimeFxChain()) {
    components_.push_back(std::make_unique<DistortionSection>(synth));
    components_.push_back(std::make_unique<DelaySection>(synth));
    components_.push_back(std::make_unique<ChrousSection>(synth));
    components_.push_back(std::make_unique<ReverbSection>(synth));
    for (const auto& e : components_) {
        order_.push_back(e.get());
        addAndMakeVisible(*e);
        e->addMouseListener(this, false);
    }
    fxchain_.AddListener(this);
    OnReload(&fxchain_);
}

TimeFxChain::~TimeFxChain() {
    fxchain_.RemoveListener(this);
}

void TimeFxChain::resized() {
    auto b = getLocalBounds().toFloat();
    auto num_fx = static_cast<float>(order_.size());
    auto section_width = b.getWidth() / num_fx;
    for (int i = 0; i < order_.size(); ++i) {
        order_[i]->setBounds(juce::Rectangle{ i * section_width, 0.0f, section_width, b.getHeight() }.toNearestInt());
    }
}

void TimeFxChain::MoveUp(int index) {
    if (IsFirstSection(index)) {
        return;
    }
    std::swap(order_[index], order_[index - 1]);
    fxchain_.MoveUp(index);
    resized();
}

void TimeFxChain::MoveDown(int index) {
    if (IsLastSection(index)) {
        return;
    }
    std::swap(order_[index], order_[index + 1]);
    fxchain_.MoveDown(index);
    resized();
}

void TimeFxChain::mouseDrag(const juce::MouseEvent& e) {
    if (e.eventComponent == this) {
        return;
    }
    dragger_.dragComponent(e.eventComponent, e, nullptr);

    auto bound = e.eventComponent->getBounds();
    auto center = bound.getCentre();
    center.x = std::clamp(center.x, 0, getWidth());
    e.eventComponent->setBounds(bound.withCentre(center).withY(0));

    auto tmp = std::distance(order_.begin(), std::ranges::find(order_, e.eventComponent));
    auto idx = static_cast<int>(tmp);
    if (IsFirstSection(idx)) {
        auto curr_x = order_[idx]->getBounds().getCentreX();
        auto next_x = order_[idx + 1]->getBounds().getTopLeft().x;
        if (curr_x > next_x) {
            MoveDown(idx);
        }
    }
    else if (IsLastSection(idx)) {
        auto curr_x = order_[idx]->getBounds().getCentreX();
        auto prev_x = order_[idx - 1]->getBounds().getTopRight().x;
        if (curr_x < prev_x) {
            MoveUp(idx);
        }
    }
    else {
        auto curr_x = order_[idx]->getBounds().getCentreX();
        auto prev_x = order_[idx - 1]->getBounds().getTopRight().x;
        auto next_x = order_[idx + 1]->getBounds().getTopLeft().x;
        if (curr_x < prev_x) {
            MoveUp(idx);
        }
        else if (curr_x > next_x) {
            MoveDown(idx);
        }
    }
}

void TimeFxChain::mouseDown(const juce::MouseEvent& e) {
    if (e.eventComponent == this) {
        return;
    }
    dragger_.startDraggingComponent(e.eventComponent, e);
}

void TimeFxChain::mouseUp(const juce::MouseEvent& e) {
    if (e.eventComponent == this) {
        return;
    }
    resized();
}

void TimeFxChain::OnReload(FxChain* pchain) {
    auto names = pchain->GetFxOrderTypeNames();
    std::vector<juce::Component*> new_order;
    for (auto name : names) {
        new_order.push_back(*std::ranges::find_if(order_,
                            [name](juce::Component* e) { return e->getComponentID().compare(name.data()) == 0; }));
    }
    order_ = std::move(new_order);
}
}