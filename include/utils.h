#pragma once

#include "constants.h"

#include <cstdint>
#include <functional>
#include <thread>

inline float degrees_to_radians(float degrees) {
  return degrees * PI_ / 180.0f;
}

// Fast per-thread pseudo-random number generator.
class RNG {
public:
  explicit RNG(uint32_t seed) : state(seed) {}

  inline uint32_t next() {
    uint32_t x = state;

    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;

    state = x;
    return x;
  }

  // Returns [0, 1)
  inline float uniform() {
    return float(next() >> 8) * (1.0f / 16777216.0f);
  }

private:
  uint32_t state;
};

inline RNG &rng() {
  thread_local RNG generator([] {
    const auto id =
        std::hash<std::thread::id>{}(
            std::this_thread::get_id());

    return static_cast<uint32_t>(id) ^ 0x9E3779B9u;
  }());

  return generator;
}

inline float random_float() {
  return rng().uniform();
}

inline float random_float(float min, float max) {
  return min + (max - min) * rng().uniform();
}

inline int random_int(int min, int max) {
  return min + int(
      rng().uniform() * float(max - min + 1));
}
