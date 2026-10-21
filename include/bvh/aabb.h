#pragma once

#include "interval.h"
#include "ray.h"
#include "vec3f.h"

class AABB {
public:
  Interval x, y, z;

  AABB() {}

  AABB(const Interval& x, const Interval& y, const Interval& z)
    : x(x), y(y), z(z) {
      pad_to_minimums();
    }

  AABB(const Point3f& a, const Point3f& b) {
    x = Interval(a.x, b.x);
    y = Interval(a.y, b.y);
    z = Interval(a.z, b.z);
  }

  AABB(const AABB& bbox0, const AABB& bbox1) {
    x = Interval(bbox0.x, bbox1.x);
    y = Interval(bbox0.y, bbox1.y);
    z = Interval(bbox0.z, bbox1.z);
  }

  const Interval& axis_interval(int n) const {
    if (n == 0) return x;
    if (n == 1) return y;
    return z;
  }

  bool hit(const Ray& r, Interval ray_t) const {
    const Point3f& ray_orig = r.origin();
    const Vec3f& ray_dir = r.direction();

    for (int axis = 0; axis < 3; axis++) {
      const Interval& ax = axis_interval(axis);
      const float adinv = 1.0 / ray_dir[axis];

      auto t0 = (ax.min - ray_orig[axis]) * adinv;
      auto t1 = (ax.max - ray_orig[axis]) * adinv;

      if (t0 < t1) {
        if (t0 > ray_t.min) ray_t.min = t0;
        if (t1 < ray_t.max) ray_t.max = t1;
      } else {
        if (t1 > ray_t.min) ray_t.min = t1;
        if (t0 < ray_t.max) ray_t.max = t0;
      }

      if (ray_t.max <= ray_t.min)
        return false;
    }

    return true;
  }
private:
  void pad_to_minimums() {
    double delta = 0.0001;
    if (x.size() < delta) x = x.expand(delta);
    if (y.size() < delta) y = y.expand(delta);
    if (z.size() < delta) z = z.expand(delta);
  }
};

AABB operator+(const AABB& bbox, const Vec3f& offset) {
  return AABB(bbox.x + offset.x, bbox.y + offset.y, bbox.z + offset.z);
}

AABB operator+(const Vec3f& offset, const AABB& bbox) {
  return bbox + offset;
}
