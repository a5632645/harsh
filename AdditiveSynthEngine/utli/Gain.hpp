#pragma once
#include <algorithm>
#include <cmath>

namespace mana::smath {
static constexpr float MIN_DB = -240.0F;
static constexpr float MAX_DB = 0.0F;
static constexpr float MUTE_DB = -256.0F;

inline static float gain_to_db(float gain) {
    if (gain <= 0.0F) {
        return MIN_DB;
    }

    float db = 20.0F * std::log10(gain);
    if (std::isnan(db) || std::isinf(db)) {
        return MIN_DB;
    }

    return db;
}

inline static float gain_to_db(float gain, float min_db) {
    if (gain <= 0.0F) {
        return min_db;
    }

    float db = 20.0F * std::log10(gain);
    if (std::isnan(db) || std::isinf(db)) {
        return min_db;
    }

    return std::max(min_db, db);
}

inline static float db_to_gain(float db) {
    if (db <= MIN_DB) {
        return 0.0F;
    }
    return std::pow(10.0F, db / 20.0F);
}
}