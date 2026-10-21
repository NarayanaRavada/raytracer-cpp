#include "color.h"
#include "material.h"
#include "ray.h"
#include "vec3f.h"

class Metal : public Material {
public:
  Metal(const Color &albedo, float fuzz) : albedo_(albedo), fuzz_(fuzz) {}

  bool scatter(const Ray &r, const HitRecord &rec, Color &attenuation, Ray &scattered) const override {
    Vec3f reflected = reflect(r.direction(), rec.normal);
    reflected = unit_vector(reflected) + fuzz_ * (random_unit_vector());
    scattered = Ray(rec.p, reflected);
    attenuation = albedo_;
    return (dot(scattered.direction(), rec.normal) > 0);
  }

private:
  Color albedo_;
  float fuzz_;
};
