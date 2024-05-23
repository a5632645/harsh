#include "spectrum.h"

#include <mutex>
#include "engine/partials.h"
#include "engine/synth.h"

namespace mana {
void Spectrum::Paint() {
    mana::Partials drawing_partials_;
    {
        std::scoped_lock lock{ synth_.GetSynthLock() };
        drawing_partials_ = synth_.GetDisplayOscillor().GetPartials();
    }

    for (size_t i = 0; i < mana::kNumPartials; ++i) {
        auto x_nor = drawing_partials_.pitches[i] / 150.0f;
        auto y_nor = 0.0f;
        auto y_gain = std::abs(drawing_partials_.gains[i]);
        if (y_gain > 0.0f) {
            auto y_del = std::clamp(20.0f * std::log10(y_gain), -60.0f, 20.0f);
            y_nor = (y_del + 60.0f) / 80.0f;
        }
        int x = static_cast<int>(bound_.x + bound_.width * x_nor);
        int y = static_cast<int>(bound_.y + bound_.height * (1.0f - y_nor));
        DrawLine(x, y, x, bound_.y + bound_.height, WHITE);
    }
}

void Spectrum::SetBounds(Rectangle bound) {
    bound_ = bound;
}
}