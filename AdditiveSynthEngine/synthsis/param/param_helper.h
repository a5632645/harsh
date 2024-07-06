#pragma once

#include <ranges>
#include "engine/poly_param.h"

namespace mana {
namespace helper {
template<std::ranges::range Range, class P>
    requires std::same_as<ModuFloatParameter*, std::ranges::range_value_t<Range>>
auto GetAlterParamValue(Range const& arg_arr, P) {
    auto val = arg_arr[P::kArgIdx];
    return P::ConvertFrom01(val->Get01Value());
}

template<class P>
auto GetAlterParamValue(ModuFloatParameter& val, P) {
    return P::ConvertFrom01(val.Get01Value());
}

template<class P>
auto GetAlterParamValue(ModuFloatParameter* val, P) {
    return P::ConvertFrom01(val->Get01Value());
}
}
}