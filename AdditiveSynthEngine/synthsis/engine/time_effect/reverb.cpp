#include "reverb.h"

#include "param/timefx/reverb_param.h"

namespace mana {
void TimeReverb::Init(float sample_rate, float update_rate) {
    FDNInit(&fdn_reverb_data_);
}

void TimeReverb::PrepareParams(ModulableParams& p) {
    enable_ = p.GetParam<BoolParameter>("timefx.reverb.enable");
    damp_ = p.GetModuFloatParam(param::TimeReverb_Damp::kId);
    dry_ = p.GetModuFloatParam(param::TimeReverb_Dry::kId);
    roomsize_ = p.GetModuFloatParam(param::TimeReverb_RoomSize::kId);
    wet_ = p.GetModuFloatParam(param::TimeReverb_Wet::kId);
    width_ = p.GetModuFloatParam(param::TimeReverb_Width::kId);

    distribution_ = p.GetModuFloatParam(param::TimeReverb_Distribution::kId);
    separate_ = p.GetModuFloatParam(param::TimeReverb_Separate::kId);
    feedback2_ = p.GetModuFloatParam(param::TimeReverb_Feedback2::kId);
    feedback3_ = p.GetModuFloatParam(param::TimeReverb_Feedback3::kId);
    freq_shift_ = p.GetModuFloatParam(param::TimeReverb_FreqShift::kId);
    fdn_matrix_ = p.GetParam<IntChoiceParameter>(param::TimeReverb_FdnMatrix::kId);
    reverb_type_ = p.GetParam<IntChoiceParameter>(param::TimeReverb_ReverbType::kId);
}

void TimeReverb::OnUpdateTick() {
    if (!enable_->GetBool()) return;

    auto type = param::TimeReverb_ReverbType::GetEnum(reverb_type_->GetInt());
    using rt = param::TimeReverb_ReverbType::ParamEnum;

    switch (type) {
    case rt::kFreeverb:
        reverb_.setdamp(damp_->GetValue());
        reverb_.setdry(dry_->GetValue());
        reverb_.setroomsize(roomsize_->GetValue());
        reverb_.setwet(wet_->GetValue());
        reverb_.setwidth(width_->GetValue());
        break;
    case rt::kFdnReverb:
    {
        auto fdn_matrix = fdn_matrix_->GetInt();
        if (old_fdn_matrix_ != fdn_matrix) {
            old_fdn_matrix_ = fdn_matrix;
            using fmt = param::TimeReverb_FdnMatrix::ParamEnum;
            auto curr_type = param::TimeReverb_FdnMatrix::GetEnum(fdn_matrix);
            switch (curr_type) {
            case fmt::kHadamard:
                FDNApplyHadamardMatrix(&fdn_reverb_data_);
                break;
            case fmt::kHouseholder:
                FDNApplyHouseholderMatrix(&fdn_reverb_data_);
                break;
            case fmt::kRandom:
                FDNApplyRandomMatrix(&fdn_reverb_data_);
                break;
            default:
                assert(false);
                break;
            }
        }
        auto roomsize = std::max(roomsize_->GetValue(), 0.03f);
        SetFDNRoomSize(&fdn_reverb_data_, roomsize, roomsize, distribution_->GetValue());
        SetFDNSeparate(&fdn_reverb_data_, separate_->GetValue());
        SetFDNDryWet(&fdn_reverb_data_, dry_->GetValue(), wet_->GetValue());
        SetFDNFeedback(&fdn_reverb_data_, 0.0f, feedback2_->GetValue(), feedback3_->GetValue());
        SetFDNFreqShift(&fdn_reverb_data_, freq_shift_->GetValue());
        break;
    }
    default:
        assert(false);
        break;
    }
}

void TimeReverb::Process(float* pbuffer, int num) {
    if (!enable_->GetBool()) return;

    auto type = param::TimeReverb_ReverbType::GetEnum(reverb_type_->GetInt());
    using rt = param::TimeReverb_ReverbType::ParamEnum;
    switch (type) {
    case rt::kFdnReverb:
        for (int i = 0; i < num; ++i) {
            StereoFloat a;
            a.l = pbuffer[i];
            a.r = pbuffer[i];
            a = FDNProc(&fdn_reverb_data_, a);
            pbuffer[i] = a.l;
        }
        break;
    case rt::kFreeverb:
        reverb_.processmix(pbuffer, pbuffer, pbuffer, pbuffer, num, 1);
        break;
    default:
        assert(false);
        break;
    }
}
}