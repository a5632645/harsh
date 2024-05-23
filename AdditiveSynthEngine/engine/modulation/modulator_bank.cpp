#include "modulator_bank.h"

#include <cassert>
#include <ranges>
#include <algorithm>
#include "lfo.h"
#include "envelop.h"

namespace mana {
ModulatorBank::ModulatorBank() {
    modulators_.emplace_back(std::make_shared<LFO>("lfo0", 0));
    modulators_.emplace_back(std::make_shared<LFO>("lfo1", 1));
    modulators_.emplace_back(std::make_shared<LFO>("lfo2", 2));
    modulators_.emplace_back(std::make_shared<LFO>("lfo3", 3));
    modulators_.emplace_back(std::make_shared<LFO>("lfo4", 4));
    modulators_.emplace_back(std::make_shared<LFO>("lfo5", 5));
    modulators_.emplace_back(std::make_shared<LFO>("lfo6", 6));
    modulators_.emplace_back(std::make_shared<LFO>("lfo7", 7));

    for (int i = 0; i < 8; ++i) {
        modulators_.emplace_back(std::make_shared<Envelop>(std::format("env{}", i), i));
    }
}

Modulator* ModulatorBank::GetModulatorPtr(std::string_view id) {
    auto it = std::ranges::find_if(modulators_,
                                   [id](const std::shared_ptr<Modulator>& modu) {
        return modu.get()->get_id() == id;
    });
    assert(it != modulators_.cend());
    return it->get();
}

std::vector<std::string_view> ModulatorBank::GetModulatorsIds() const {
    auto view = modulators_ | std::ranges::views::transform(
        [](const std::shared_ptr<Modulator>& modu) {
        return modu.get()->get_id();
    });
    return { view.begin(),view.end() };
}

void ModulatorBank::Init(float sample_rate, float update_rate) {
    for (auto& p : modulators_) {
        p->Init(sample_rate, update_rate);
    }
}

void ModulatorBank::PrepareParams(OscillorParams & params) {
    for (auto& m : modulators_) {
        m->PrepareParams(params);
    }
}

void ModulatorBank::OnUpdateTick() {
    for (auto& p : modulators_) {
        p->OnUpdateTick();
    }
}

void ModulatorBank::OnNoteOn(int note) {
    for (auto& p : modulators_) {
        p->OnNoteOn(note);
    }
}

void ModulatorBank::OnNoteOff() {
    for (auto& p : modulators_) {
        p->OnNoteOff();
    }
}
}