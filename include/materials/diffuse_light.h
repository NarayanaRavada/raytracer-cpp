#include "color.h"
#include "material.h"
#include "vec3f.h"

class DiffuseLight: public Material {
public:
  DiffuseLight(const Color &emission)
    : emission_(emission) {}

  Color emitted(double u, double v, const Point3f &p) const override {
    return emission_;
  }

  bool scatter(const Ray &r, const HitRecord &rec, Color &attenuation, Ray &scattered) const override {
    return false;
  }

private:
  Color emission_;
};
