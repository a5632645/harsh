#include "phase.h"

#include <cassert>
#include <numbers>
#include <complex>
#include "utli/warp.h"
#include "engine/oscillor_param.h"

namespace mana {
void PhaseProcessor::Init(float sample_rate, float update_rate) {
}

void PhaseProcessor::OnUpdateTick() {
    process_type_ = param::PhaseType::GetEnum(type_->GetInt());
    process_arg0_ = arg0_->GetValue();
    process_arg1_ = arg1_->GetValue();

    if (note_on_once_flag_) {
        for (int i = 0; i < kNumPartials; ++i) {
            auto bit_r = static_cast<float>(rand_()) / static_cast<float>(std::random_device::max());
            auto r = 2.0f * bit_r - 1.0f;

            auto omega = std::numbers::pi_v<float> *r; // -pi to pi
            random_phases_[i] = omega;
        }
    }
}

void PhaseProcessor::PrepareParams(OscillorParams & params) {
    type_ = params.GetParam<IntChoiceParameter>("phase.type");
    arg0_ = params.GetPolyFloatParam("phase.arg0");
    arg1_ = params.GetPolyFloatParam("phase.arg1");
}

void PhaseProcessor::OnNoteOn(int note) {
    note_on_once_flag_ = true;
}

void PhaseProcessor::OnNoteOff() {
}

void PhaseProcessor::Process(Partials& partials) {
    using enum param::PhaseType::ParamEnum;

    switch (process_type_) {
    case kContinue:
        // do nothing
        break;
    case kResetOnce:
        DoResetOnce(partials);
        break;
    case kDispersion:
        DoDispersion(partials);
        break;
    case kSpectralRandom:
        DoSpectralRandom(partials);
        break;
    case kPowDistribute:
        DoPowDistribute(partials);
        break;
    default:
        assert("unimplemented function?");
        break;
    }

    note_on_once_flag_ = false;
}

inline void PhaseProcessor::DoResetOnce(Partials& partials) {
    if (note_on_once_flag_) {
        note_on_once_flag_ = false;

        partials.update_phase = true;
        partials.phases.fill({ 1.0f, 0.0f });
    }
}

inline void PhaseProcessor::DoDispersion(Partials& partials) {
    if (note_on_once_flag_) {
        note_on_once_flag_ = false;

        auto dispersion_v = param::PhaseDispersion_Amount::GetNumber(process_arg0_);
        auto cpx = param::PhaseDispersion_Warp::GetNumber(process_arg1_);
        for (int i = 0; i < kNumPartials; ++i) {
            auto e = static_cast<float>(i);
            auto r = e / kNumPartials;
            auto r_warp = utli::warp::ParabolaWarp(r, cpx);
            auto omega = dispersion_v * r_warp * e * std::numbers::pi_v<float> *5.0f;
            partials.phases[i] = std::polar(1.0f, omega);
        }
    }
}

inline void PhaseProcessor::DoSpectralRandom(Partials& partials) {
    if (note_on_once_flag_) {
        note_on_once_flag_ = false;

        auto amount = param::PhaseSpRandom_Amount::GetNumber(process_arg0_);
        for (int i = 0; i < kNumPartials; ++i) {
            partials.phases[i] = std::polar(1.0f, random_phases_[i] * amount);
        }
    }
}

inline void PhaseProcessor::DoPowDistribute(Partials& partials) {
    if (note_on_once_flag_) {
        note_on_once_flag_ = false;

        auto pro_v = param::PhasePowDist_Pow::GetNumber(process_arg0_);
        for (int i = 0; i < kNumPartials; ++i) {
            auto e = static_cast<float>(i);
            auto omega = std::pow(e, pro_v) * std::numbers::pi_v<float>;
            partials.phases[i] = std::polar(1.0f, omega);
        }
    }
}
}