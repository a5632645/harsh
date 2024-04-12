#pragma once

namespace mana::param {
struct HardSyncParam {
    float sync{};
};

struct TimberProcParam {
    HardSyncParam hard_sync;
};
}