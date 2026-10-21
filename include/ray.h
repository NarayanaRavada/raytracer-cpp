#pragma once

#include "vec3f.h"

struct Ray {
  Ray() {};

  Ray(const Point3f &origin, const Vec3f &direction)
      : origin_(origin), direction_(direction) {}

  const Point3f &origin() const { return origin_; }
  const Point3f &direction() const { return direction_; }

  Point3f at(float t) const { return origin_ + direction_ * t; }

private:
  Point3f origin_;
  Vec3f direction_;
};
