#pragma once

#include "utils.h"

#include <cassert>
#include <cmath>
#include <concepts>
#include <cstddef>

struct Vec3f {
  union {
    struct {
      float x, y, z;
    };
    float data[3];
  };

  Vec3f() {};

  Vec3f(float k) : x(k), y(k), z(k) {}

  Vec3f(float x, float y, float z) : x(x), y(y), z(z) {}

  float &operator[](size_t n) {
    assert(n <= 2);
    return data[n];
  }
  const float &operator[](size_t n) const { return data[n]; }

  float length_squared() const { return x * x + y * y + z * z; }

  float length() const { return std::sqrt(length_squared()); }

  bool near_zero() const {
    auto s = 1e-8;
    return (std::fabs(x) < s) && (std::fabs(y) < s) && (std::fabs(z) < s);
  }

  Vec3f &operator+=(const Vec3f &v) {
    x += v.x;
    y += v.y;
    z += v.z;
    return *this;
  }

  Vec3f &operator-=(const Vec3f &v) {
    x -= v.x;
    y -= v.y;
    z -= v.z;
    return *this;
  }

  Vec3f &operator*=(const Vec3f &v) {
    x *= v.x;
    y *= v.y;
    z *= v.z;
    return *this;
  }

  Vec3f &operator/=(const Vec3f &v) {
    x /= v.x;
    y /= v.y;
    z /= v.z;
    return *this;
  }

  static Vec3f random() {
    return Vec3f(random_float(), random_float(), random_float());
  }

  static Vec3f random(float min, float max) {
    return Vec3f(random_float(min, max), random_float(min, max),
                 random_float(min, max));
  }
};

using Point3f = Vec3f;

// add, sub, mul, div

inline Vec3f operator+(const Vec3f &a, const Vec3f &b) {
  return Vec3f(a.x + b.x, a.y + b.y, a.z + b.z);
}

inline Vec3f operator-(const Vec3f &a, const Vec3f &b) {
  return Vec3f(a.x - b.x, a.y - b.y, a.z - b.z);
}

inline Vec3f operator*(const Vec3f &a, const Vec3f &b) {
  return Vec3f(a.x * b.x, a.y * b.y, a.z * b.z);
}

inline Vec3f operator/(const Vec3f &a, const Vec3f &b) {
  return Vec3f(a.x / b.x, a.y / b.y, a.z / b.z);
}

// utilities

inline float dot(const Vec3f &a, const Vec3f &b) {
  return a.x * b.x + a.y * b.y + a.z * b.z;
}

inline Vec3f cross(const Vec3f &u, const Vec3f &v) {
  return Vec3f(u.y * v.z - u.z * v.y, u.z * v.x - u.x * v.z,
               u.x * v.y - u.y * v.x);
}

inline Vec3f unit_vector(const Vec3f &a) { return a / a.length(); }

inline Vec3f random_unit_vector() {
  while (true) {
    auto p = Vec3f::random(-1, 1);
    auto lensq = p.length_squared();
    if (1e-120 < lensq && lensq <= 1) {
      return p / p.length();
    }
  }
}

inline Vec3f random_square() {
  return Vec3f(random_float() - 0.5, random_float() - 0.5, 0);
}

inline Vec3f random_vector_on_hemisphere(const Vec3f &normal) {
  Vec3f on_unit_sphere = random_unit_vector();
  if (dot(normal, on_unit_sphere) > 0.0) {
    return on_unit_sphere;
  } else {
    return -1 * on_unit_sphere;
  }
}

inline Vec3f random_in_unit_disk() {
  while (true) {
    auto p = Vec3f(random_float(-1, 1), random_float(-1, 1), 0);
    if (p.length_squared() < 1) {
      return p;
    }
  }
}

// optics

inline Vec3f reflect(const Vec3f &v, const Vec3f &n) {
  return v - 2 * dot(v, n) * n;
}

inline Vec3f refract(const Vec3f &uv, const Vec3f &n,
                     float relative_refractive_index) {
  auto cos_theta = std::fmin(dot(-1 * uv, n), 1.0);
  Vec3f r_out_perpendicular = relative_refractive_index * (uv + cos_theta * n);
  Vec3f r_out_parallel =
      -std::sqrt(std::fabs(1.0 - r_out_perpendicular.length_squared())) * n;
  return r_out_perpendicular + r_out_parallel;
}

// function on all

template <typename F>
concept FloatFunc = requires(F f, float x) {
  { f(x) } -> std::convertible_to<float>;
};

template <FloatFunc F> inline Vec3f map(const Vec3f &v, F&& f) {
  return Vec3f(f(v.x), f(v.y), f(v.z));
}
