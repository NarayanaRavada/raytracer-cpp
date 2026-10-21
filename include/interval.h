#pragma once

#include "constants.h"
#include <cmath>

class Interval {
public:
  float min, max;

  Interval() : min(-INFINITY_), max(INFINITY_) {}

  Interval(float a, float b) : min(std::fmin(a, b)), max(std::fmax(a, b)) {}

  Interval(const Interval& a, const Interval& b) {
    min = a.min <= b.min ? a.min : b.min;
    max = a.max >= b.max ? a.max : b.max;
  }

  float size() const { return max - min; }
  bool contains(float x) const { return min <= x && x <= max; }
  bool surrounds(float x) const { return min < x && x < max; }

  float clamp(float x) const {
    if (x <= min)
      return min;
    if (x >= max)
      return max;
    return x;
  }

  Interval expand(float delta) {
    auto padding = delta / 2;
    return Interval(min - padding, max + padding);
  }
};

const Interval EMPTY_INTERVAL_(INFINITY_, -INFINITY_);
const Interval INFINITE_INTERVAL_(-INFINITY_, INFINITY_);

Interval operator+(const Interval& I, float displacement) {
  return Interval(I.min + displacement, I.max + displacement);
}

Interval operator+(float displacement, const Interval& I) {
  return I + displacement;
}
