#include <array>
#include "formant_filter.h"
#include "engine/oscillor_param.h"
#include "param/filter_param.h"
#include "utli/convert.h"

namespace mana {
struct VowelInfo {
    std::array<float, 5> freqs;
    std::array<float, 5> amps;
    std::array<float, 5> bws;
};

template<size_t N>
static constexpr auto ConvertToPitchTable(const std::array<VowelInfo, N>& table) {
    std::array<FormantFilter::ConvertVowlInfo, N> out{};
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < 5; ++j) {
            out[i].cut_pitch[j] = utli::cp::FreqToPitch(table[i].freqs[j]);
            out[i].p_begin[j] = utli::cp::FreqToPitch(table[i].freqs[j] - table[i].bws[j] / 2.0f);
            out[i].p_end[j] = utli::cp::FreqToPitch(table[i].freqs[j] + table[i].bws[j] / 2.0f);
            out[i].amps[j] = table[i].amps[j];
            out[i].magic_coef[j] = 1.0f / std::min(-0.01f, out[i].p_begin[j] - out[i].p_end[j]);
        }
    }
    return out;
}

// https://www.classes.cs.uchicago.edu/archive/1999/spring/CS295/Computing_Resources/Csound/CsManual3.48b1.HTML/Appendices/table3.html
static constexpr std::array kSopranoVowels = ConvertToPitchTable(std::array{
    VowelInfo {
        {800.0f, 1150.0f,2900.0f,3900.0f,4950.0f},
        {0.0f,-6.0f,-32.0f,-20.0f,-50.0f},
        {80.0f,90.0f,120.0f,130.0f,140.0f}
    },
    VowelInfo{
        {350.0f,2000.0f,2800.0f,3600.0f,4950.0f,},
        {0.0f,-20.0f,-15.0f,-40.0f,-56.0f,},
        {60.0f,100.0f,120.0f,150.0f,200.0f,}
    },
    VowelInfo{
        {270.0f,2140.0f,2950.0f,3900.0f,4950.0f,},
        {0.0f,-12.0f,-26.0f,-26.0f,-44.0f,},
        {60.0f,90.0f,100.0f,120.0f,120.0f,}
    },
    VowelInfo{
        {450.0f,800.0f,2830.0f,3800.0f,4950.0f,},
        {0.0f,-11.0f,-22.0f,-22.0f,-50.0f,},
        {70.0f,80.0f,100.0f,130.0f,135.0f,}
    },
    VowelInfo{
        {325.0f,700.0f,2700.0f,3800.0f,4950.0f,},
        {0.0f,-16.0f,-35.0f,-40.0f,-60.0f,},
        {50.0f,60.0f,170.0f,180.0f,200.0f,}
    }
                                                                 });

static constexpr std::array kAltoVowel = ConvertToPitchTable(std::array{
    VowelInfo{
        {800.0f,1150.0f,2800.0f,3500.0f,4950.0f,},
        {0.0f,-4.0f,-20.0f,-36.0f,-60.0f,},
        {80.0f,90.0f,120.0f,130.0f,140.0f,}
    },
    VowelInfo{
        {400.0f,1600.0f,2700.0f,3300.0f,4950.0f,},
        {0.0f,-24.0f,-30.0f,-35.0f,-60.0f,},
        {60.0f,80.0f,120.0f,150.0f,200.0f,}
    },
    VowelInfo{
        {350.0f,1700.0f,2700.0f,3700.0f,4950.0f,},
        {0.0f,-20.0f,-30.0f,-36.0f,-60.0f,},
        {50.0f,100.0f,120.0f,150.0f,200.0f,}
    },
    VowelInfo{
        {450.0f,800.0f,2830.0f,3500.0f,4950.0f,},
        {0.0f,-9.0f,-16.0f,-28.0f,-55.0f,},
        {70.0f,80.0f,100.0f,130.0f,135.0f,}
    },
    VowelInfo{
        {325.0f,700.0f,2530.0f,3500.0f,4950.0f,},
        {0.0f,-12.0f,-30.0f,-40.0f,-64.0f,},
        {50.0f,60.0f,170.0f,180.0f,200.0f,}
    }
                                                             });

static constexpr std::array kCountertenorVowel = ConvertToPitchTable(std::array{
    VowelInfo{
        {660.0f,1120.0f,2750.0f,3000.0f,3350.0f,},
        {0.0f,-6.0f,-23.0f,-24.0f,-38.0f,},
        {80.0f,90.0f,120.0f,130.0f,140.0f,}
    },
    VowelInfo{
        {440.0f,1800.0f,2700.0f,3000.0f,3300.0f,},
        {0.0f,-14.0f,-18.0f,-20.0f,-20.0f,},
        {70.0f,80.0f,100.0f,120.0f,120.0f,}
    },
    VowelInfo{
        {270.0f,1850.0f,2900.0f,3350.0f,3590.0f,},
        {0.0f,-24.0f,-24.0f,-36.0f,-36.0f,},
        {40.0f,90.0f,100.0f,120.0f,120.0f,}
    },
    VowelInfo{
        {430.0f,820.0f,2700.0f,3000.0f,3300.0f,},
        {0.0f,-10.0f,-26.0f,-22.0f,-34.0f,},
        {40.0f,80.0f,100.0f,120.0f,120.0f,}
    },
    VowelInfo{
        {370.0f,630.0f,2750.0f,3000.0f,3400.0f,},
        {0.0f,-20.0f,-23.0f,-30.0f,-34.0f,},
        {40.0f,60.0f,100.0f,120.0f,120.0f,}
    },
                                                                     });

static constexpr std::array kTenorVowel = ConvertToPitchTable(std::array{
    VowelInfo{
        {650.0f,1080.0f,2650.0f,2900.0f,3250.0f,},
        {0.0f,-6.0f,-7.0f,-8.0f,-22.0f,},
        {80.0f,90.0f,120.0f,130.0f,140.0f,}
    },
    VowelInfo{
        {400.0f,1700.0f,2600.0f,3200.0f,3580.0f,},
        {0.0f,-14.0f,-12.0f,-14.0f,-20.0f,},
        {70.0f,80.0f,100.0f,120.0f,120.0f,}
    },
    VowelInfo{
        {290.0f,1870.0f,2800.0f,3250.0f,3540.0f,},
        {0.0f,-15.0f,-18.0f,-20.0f,-30.0f,},
        {40.0f,90.0f,100.0f,120.0f,120.0f,}
    },
    VowelInfo{
        {400.0f,800.0f,2600.0f,2800.0f,3000.0f,},
        {0.0f,-10.0f,-12.0f,-12.0f,-26.0f,},
        {40.0f,80.0f,100.0f,120.0f,120.0f,}
    },
    VowelInfo{
        {350.0f,600.0f,2700.0f,2900.0f,3300.0f,},
        {0.0f,-20.0f,-17.0f,-14.0f,-26.0f,},
        {40.0f,60.0f,100.0f,120.0f,120.0f,}
    }
                                                              });

static constexpr std::array kBassVowel = ConvertToPitchTable(std::array{
    VowelInfo{
        {600.0f,1040.0f,2250.0f,2450.0f,2750.0f,},
        {0.0f,-7.0f,-9.0f,-9.0f,-20.0f,},
        {60.0f,70.0f,110.0f,120.0f,130.0f,}
    },
    VowelInfo{
        {400.0f,1620.0f,2400.0f,2800.0f,3100.0f,},
        {0.0f,-12.0f,-9.0f,-12.0f,-18.0f,},
        {40.0f,80.0f,100.0f,120.0f,120.0f,}
    },
    VowelInfo{
        {250.0f,1750.0f,2600.0f,3050.0f,3340.0f,},
        {0.0f,-30.0f,-16.0f,-22.0f,-28.0f,},
        {60.0f,90.0f,100.0f,120.0f,120.0f,}
    },
    VowelInfo{
        {400.0f,750.0f,2400.0f,2600.0f,2900.0f,},
        {0.0f,-11.0f,-21.0f,-20.0f,-40.0f,},
        {40.0f,80.0f,100.0f,120.0f,120.0f,}
    },
    VowelInfo{
        {350.0f,600.0f,2400.0f,2675.0f,2950.0f,},
        {0.0f,-20.0f,-32.0f,-28.0f,-36.0f,},
        {40.0f,80.0f,100.0f,120.0f,120.0f,}
    }
                                                             });

inline static const auto& GetVowelInfoArray(param::VowelFilter_Singer::ParamEnum e) {
    using sgr = param::VowelFilter_Singer::ParamEnum;

    switch (e) {
    case sgr::kAlto:
        return kAltoVowel;
    case sgr::kBass:
        return kBassVowel;
    case sgr::kCountertenor:
        return kCountertenorVowel;
    case sgr::kSoprano:
        return kSopranoVowels;
    case sgr::kTenor:
        return kTenorVowel;
    default:
        assert(false);
        return kTenorVowel;
    }
}

void FormantFilter::Init(float sample_rate, float update_rate) {
    inv_sample_rate_ = 1.0f / sample_rate;
}

void FormantFilter::PrepareParams(OscillorParams & params) {
    arg_formant_shift_ = params.GetPolyFloatParam("filter{}.arg{}", idx_, param::VowelFilter_Formant::kArgIdx);
    arg_resonance_ = params.GetPolyFloatParam("filter{}.arg{}", idx_, param::VowelFilter_Resonance::kArgIdx);
    arg_singer_ = params.GetPolyFloatParam("filter{}.arg{}", idx_, param::VowelFilter_Singer::kArgIdx);
    arg_slope_ = params.GetPolyFloatParam("filter{}.arg{}", idx_, param::VowelFilter_Slope::kArgIdx);
    arg_select_ = params.GetPolyFloatParam("filter{}.arg{}", idx_, param::VowelFilter_Select::kArgIdx);
}

void FormantFilter::Process(Partials& partials, std::vector<float>& out) {
    auto slope = param::VowelFilter_Slope::GetNumber(arg_slope_->GetValue());
    auto resonance = param::VowelFilter_Resonance::GetNumber(arg_resonance_->GetValue());
    auto shift = param::VowelFilter_Formant::GetNumber(arg_formant_shift_->GetValue());

    std::ranges::fill(out, 0.0f);
    for (int filter_idx = 0; filter_idx < 5; ++filter_idx) {
        auto p_begin = vowel_info_.p_begin[filter_idx] + shift;
        auto p_end = vowel_info_.p_end[filter_idx] + shift;
        auto cut_pitch = vowel_info_.cut_pitch[filter_idx] + shift;
        auto amptide = vowel_info_.amps[filter_idx];
        auto parabola_a = vowel_info_.magic_coef[filter_idx] * slope;

        for (int i = 0; i < kNumPartials; ++i) {
            float gain = 0.0f;
            float partial_p = partials.pitches[i];

            if (partial_p < p_begin) {
                gain = (p_begin - partial_p) * (-slope);
            }
            else if (partial_p > p_end) {
                gain = (partial_p - p_end) * (-slope);
            }
            else {
                auto xxx = partial_p - cut_pitch;
                gain = parabola_a * xxx * xxx;
            }

            out[i] += utli::DbToGain(gain + amptide + 2.0f * slope);
        }
    }
}

void FormantFilter::OnUpdateTick() {
    using sgr = param::VowelFilter_Singer::ParamEnum;

    auto singer_enum = param::VowelFilter_Singer::GetEnum(arg_singer_->Get01Value());
    auto select = param::VowelFilter_Select::GetChoiceIndex(arg_select_->Get01Value());

    const auto& singer_infos = GetVowelInfoArray(singer_enum);
    const auto& vowel_info = singer_infos.at(select);
    vowel_info_ = vowel_info;
}
}