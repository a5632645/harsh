#pragma once

#include <ranges>
#include "engine/poly_param.h"

namespace mana {
namespace helper {
template<std::ranges::range T, class P>
    requires std::same_as<PolyModuFloatParameter*, std::ranges::range_value_t<T>>
auto GetAlterParamValue(T const& arg_arr, P) {
    auto val = arg_arr[P::kArgIdx];
    return P::GetNumber(val->Get01Value());
}
}
}