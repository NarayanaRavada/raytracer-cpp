#pragma once

#include "color.h"
#include "texture.h"
#include "vec3f.h"

#include "perlin.h"

class Noise: public Texture {
  public:
    Noise(double scale) : scale(scale) {}
    Color value(double u, double v, const Point3f& p) const override {
      return Color(.5, .5, .5) * (1 + std::sin(scale * p.z + 10 * perlin.turb(p, 7)));
    }

  private:
    Perlin perlin;
    double scale;
};
