#pragma once

#include "color.h"
#include "texture.h"

class SolidColor : public Texture {
public:
  SolidColor(const Color &albedo) : albedo_(albedo) {}

  SolidColor(float red, float green, float blue)
      : albedo_(Color(red, green, blue)) {}

  Color value(double u, double v, const Point3f &p) const override {
    return albedo_;
  }

private:
  Color albedo_;
};
