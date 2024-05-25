#include "modulators_layout.h"

#include "engine/synth.h"

namespace mana {
ModulatorsLayout::ModulatorsLayout(Synth& synth) {
    const auto& ids = synth.GetModulatorIds();
    for (auto id : ids) {
        buttons_.emplace_back(id.data());

        if (id.starts_with("lfo")) {
            auto idx_str = id.substr(3);
            int d{};
            auto [ptr, err] = std::from_chars(idx_str.cbegin()._Unwrapped(), idx_str.cend()._Unwrapped(), d);
            assert(err == std::error_code{});
            layouts_.emplace_back(std::make_unique<LfoLayout>(synth, d, id));
        }
        else if (id.starts_with("env")) {
            auto idx_str = id.substr(3);
            int d{};
            auto [ptr, err] = std::from_chars(idx_str.cbegin()._Unwrapped(), idx_str.cend()._Unwrapped(), d);
            assert(err == std::error_code{});
            layouts_.emplace_back(std::make_unique<EnvelopLayout>(synth, d, id));
        }
    }
}

void ModulatorsLayout::Paint() {
    for (int i = 0; auto & b : buttons_) {
        if (b.Show()) {
            current_layout_ = i;
        }
        ++i;
    }
    layouts_[current_layout_]->Paint();
}

void ModulatorsLayout::SetBounds(Rectangle bound) {
    for (int i = 0; auto & b : buttons_) {
        int y = i / 8;
        int xx = i % 8;
        b.SetBounds({ bound.x + 50 * xx, bound.y + 16 * y, 50, 16 });
        ++i;
    }

    for (auto& l : layouts_) {
        l->SetBounds({ bound.x, bound.y + 32, bound.width, bound.height - 32 });
    }
}
}