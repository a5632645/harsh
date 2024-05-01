#pragma once

namespace mana {
static constexpr int kNumOscillors = 1;
static constexpr int kNumPartials = 256;
static constexpr auto kFFtSize = kNumPartials * 4;
static constexpr auto kHarmonics = kFFtSize / 2;
static constexpr auto kFFtHop = kFFtSize / 4;
}