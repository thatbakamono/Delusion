#pragma once

#include <random>

static std::random_device s_randomDevice;
static std::mt19937_64 s_random(s_randomDevice());
static std::uniform_int_distribution<uint64_t> s_uniformDistribution;

class Random {
    public:
        [[nodiscard]] static uint64_t randomU64() {
            return s_uniformDistribution(s_random);
        }
};
