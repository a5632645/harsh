#pragma once

namespace mana::param {

struct DualSawParam {
    float ratio_;
    float beating_rate_;
    float saw_square_;
};

struct TimberParam {
    DualSawParam dual_saw;
};

}