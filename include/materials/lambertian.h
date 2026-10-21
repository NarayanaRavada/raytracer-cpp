#include "color.h"
#include "material.h"
#include "ray.h"
#include "texture.h"
#include "textures/solid_color.h"
#include "vec3f.h"

#include <memory>

class Lambertian : public Material {
public:
  Lambertian(std::shared_ptr<Texture> texture) : texture_(texture) {}

  Lambertian(const Color &albedo)
      : texture_(std::make_shared<SolidColor>(albedo)) {}

  bool scatter(const Ray &r, const HitRecord &rec, Color &attenuation, Ray &scattered) const override {
    auto scatter_direction = rec.normal + random_unit_vector();

    if (scatter_direction.near_zero()) {
      scatter_direction = rec.normal;
    }

    scattered = Ray(rec.p, scatter_direction);
    attenuation = texture_->value(rec.u, rec.v, rec.p);
    return true;
  }

private:
  std::shared_ptr<Texture> texture_;
};
