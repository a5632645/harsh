#include "engine/synth.h"
#include "ui/Knob.h"

namespace mana {
class ResynthsisLayout {
public:
    ResynthsisLayout(Synth& synth);

    void Paint();
    void SetBounds(int x, int y, int w, int h);
private:
    Synth synth_;
    std::array<Knob, 7> arg_knobs_;
};
}