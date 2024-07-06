#include "fx_chain.h"

#include <tuple>
#include <vector>
#include "timefx_base.h"
#include "utli/listener_list.h"
#include "nlohmann/json.hpp"
#include "engine/forward_decalre.h"
#include "delay.h"
#include "distortion.h"
#include "chrous.h"
#include "reverb.h"

namespace mana {
using FxChainTuple = std::tuple<Delay, Distortion, Chrous, TimeReverb>;
}

namespace mana {
class FxChain::FxChainImpl {
public:
    FxChainImpl() {
        std::apply([this](auto&&... fxs) {
            int _[]{ (fx_order_.push_back(&fxs), 0)... };
        }, effects_);
    }

    void Init(float sample_rate, float update_rate) {
        std::apply([sample_rate, update_rate](auto&&... fxs) {
            int _[]{ (fxs.Init(sample_rate, update_rate),0)... };
        }, effects_);
    }

    void PrepareParams(ModulableParams& params) {
        std::apply([&params](auto&&... fxs) {
            int _[]{ (fxs.PrepareParams(params),0)... };
        }, effects_);
    }

    void OnUpdateTick() {
        std::apply([](auto&&... fxs) {
            int _[]{ (fxs.OnUpdateTick(),0)... };
        }, effects_);
    }

    void OnNoteOn(int note) {
        std::apply([note](auto&&... fxs) {
            int _[]{ (fxs.OnNoteOn(note),0)... };
        }, effects_);
    }

    void OnNoteOff() {
        std::apply([](auto&&... fxs) {
            int _[]{ (fxs.OnNoteOff(),0)... };
        }, effects_);
    }

    void Process(float* pbuffer, int num) {
        for (auto* f : fx_order_) {
            f->Process(pbuffer, num);
        }
    }

    nlohmann::json SaveState() const {
        nlohmann::json j;
        for (auto* t : fx_order_) {
            j.push_back(t->GetFxType());
        }
        return j;
    }

    void LoadState(const nlohmann::json& j) {
        std::vector<std::string> types;
        for (auto& t : j) {
            types.push_back(t.get<std::string>());
        }
        auto new_order = fx_order_;
        for (int curr = 0; const auto & t : types) {
            auto it = std::ranges::find_if(new_order | std::views::drop(curr), [&t](TimeFxBase* fx) {
                return fx->GetFxType() == t;
            });
            std::swap(new_order[curr], *it);
            ++curr;
        }
        fx_order_ = std::move(new_order);
    }

    void MoveFxOrder(int curr_idx, int new_idx) {
        constexpr auto size = std::tuple_size_v<decltype(effects_)>;
        assert(curr_idx >= 0 && curr_idx < size);
        assert(new_idx >= 0 && new_idx < size);
        if (curr_idx == new_idx) {
            return;
        }
        if (curr_idx > new_idx) {
            auto* old = fx_order_[curr_idx];
            std::move(fx_order_.begin() + new_idx, fx_order_.begin() + curr_idx, fx_order_.begin() + new_idx + 1);
            fx_order_[new_idx] = old;
        }
        else if (curr_idx < new_idx) {
            auto* old = fx_order_[curr_idx];
            std::move(fx_order_.begin() + curr_idx + 1, fx_order_.begin() + new_idx, fx_order_.begin() + curr_idx);
            fx_order_[new_idx] = old;
        }
    }

    void MoveUp(int tomove) {
        if (tomove == 0) {
            return;
        }
        std::swap(fx_order_[tomove], fx_order_[tomove - 1]);
    }

    void MoveDown(int tomove) {
        auto size = static_cast<int>(fx_order_.size());
        if (tomove == size - 1) {
            return;
        }
        std::swap(fx_order_[tomove], fx_order_[tomove + 1]);
    }

    std::vector<std::string_view> GetFxOrderTypeNames() const {
        std::vector<std::string_view> names;
        for (auto* t : fx_order_) {
            names.push_back(t->GetFxType());
        }
        return names;
    }
private:
    std::vector<TimeFxBase*> fx_order_;
    FxChainTuple effects_;
};
}

// ================================================================================
namespace mana {
FxChain::FxChain() {
    chain_ = std::make_unique<FxChainImpl>();
}

FxChain::~FxChain() {
    chain_ = nullptr;
}

void FxChain::Init(float sample_rate, float update_rate) {
    chain_->Init(sample_rate, update_rate);
}

void FxChain::PrepareParams(ModulableParams& params) {
    chain_->PrepareParams(params);
}

void FxChain::OnUpdateTick() {
    chain_->OnUpdateTick();
}

void FxChain::OnNoteOn(int note) {
    chain_->OnNoteOn(note);
}

void FxChain::OnNoteOff() {
    chain_->OnNoteOff();
}

void FxChain::Process(float* pbuffer, int num) {
    chain_->Process(pbuffer, num);
}

void FxChain::MoveFxOrder(int tomove, int new_idx) {
    chain_->MoveFxOrder(tomove, new_idx);
}

void FxChain::MoveUp(int tomove) {
    chain_->MoveUp(tomove);
}

void FxChain::MoveDown(int tomove) {
    chain_->MoveDown(tomove);
}

nlohmann::json FxChain::SaveState() const {
    return chain_->SaveState();
}

void FxChain::LoadState(const nlohmann::json& j) {
    chain_->LoadState(j);
    listeners_.CallListener(&Listener::OnReload, this);
}

std::vector<std::string_view> FxChain::GetFxOrderTypeNames() const {
    return chain_->GetFxOrderTypeNames();
}
}