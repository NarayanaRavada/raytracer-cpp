#pragma once

#include "color.h"
#include "texture.h"
#include <cmath>
#include <memory>

#include "solid_color.h"

class Checker : public Texture {
public:
  Checker(float scale, std::shared_ptr<Texture> even,
          std::shared_ptr<Texture> odd)
      : inv_scale_(1.0 / scale), even_(even), odd_(odd) {}

  Checker(float scale, const Color &c1, const Color &c2)
      : inv_scale_(1.0 / scale), even_(std::make_shared<SolidColor>(c1)),
        odd_(std::make_shared<SolidColor>(c2)) {}

  Color value(double u, double v, const Point3f &p) const override {
    auto x = int(std::floor(inv_scale_ * u));
    auto y = int(std::floor(inv_scale_ * v));

    bool isOdd = (x + y) & 1;
    return isOdd ? odd_->value(u, v, p) : even_->value(u, v, p);
  }

private:
  float inv_scale_;
  std::shared_ptr<Texture> even_;
  std::shared_ptr<Texture> odd_;
};
