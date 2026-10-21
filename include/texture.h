#pragma once

#include "color.h"
#include "vec3f.h"

class Texture {
public:
  virtual ~Texture() = default;

  virtual Color value(double u, double v, const Point3f &p) const = 0;
};
