#pragma once

namespace mana {
static constexpr int kNumOscillors = 1;
static constexpr int kNumPartials = 256;
static constexpr auto kFFtSize = 512;
static constexpr auto kFFtHop = kFFtSize / 4;
}