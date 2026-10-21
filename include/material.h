#pragma once

#include "color.h"
#include "hittable.h"
#include "ray.h"
#include "vec3f.h"

class Material {
public:
  virtual ~Material() = default;

  virtual bool scatter(const Ray &r, const HitRecord &rec, Color &attenuation, Ray &scattered) const = 0;

  virtual Color emitted(double u, double v, const Point3f &p) const {
    return Color(0, 0, 0);
  }
};
