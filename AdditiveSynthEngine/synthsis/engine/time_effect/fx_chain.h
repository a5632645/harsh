#pragma once

#include <vector>
#include <memory>
#include <nlohmann/json_fwd.hpp>
#include "utli/listener_list.h"
#include "engine/forward_decalre.h"

namespace mana {
class ModulableParams;
}

namespace mana {
class FxChain {
public:
    struct Listener {
        virtual ~Listener() = default;
        virtual void OnReload(FxChain* pchain) = 0;
    };

    FxChain();
    ~FxChain();

    void Init(float sample_rate, float update_rate);
    void PrepareParams(ModulableParams& params);
    void OnUpdateTick();
    void OnNoteOn(int note);
    void OnNoteOff();
    void Process(float* pbuffer, int num);
    void MoveFxOrder(int tomove, int new_idx);
    void MoveUp(int tomove);
    void MoveDown(int tomove);

    nlohmann::json SaveState() const;
    void LoadState(const nlohmann::json& j);
    std::vector<std::string_view> GetFxOrderTypeNames() const;

    void AddListener(Listener* l) { listeners_.AddListener(l); }
    void RemoveListener(Listener* l) { listeners_.RemoveListener(l); }
private:
    class FxChainImpl;
    std::unique_ptr<FxChainImpl> chain_;
    utli::ListenerList<Listener> listeners_;
};
}