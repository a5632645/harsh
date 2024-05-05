#include "param/synth_param.h"
#include "param/timber_param.h"
#include "timber_frame.h"

namespace mana {
class DynamicNoise {
public:
    void Init(float sample_rate) {}
    void Process(TimberFrame& frame) {}
    void OnUpdateTick(const SynthParam& params, int skip, int module_idx) {}
    void OnNoteOn(int note) {}
    void OnNoteOff() {}

private:
};
}