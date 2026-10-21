#include <memory>

#include "material.h"
#include "texture.h"
#include "textures/solid_color.h"

class Isotropic : public Material {
  public:
    Isotropic(const Color& albedo) : tex(std::make_shared<SolidColor>(albedo)) {}
    Isotropic(std::shared_ptr<Texture> tex) : tex(tex) {}

    bool scatter(const Ray& r_in, const HitRecord& rec, Color& attenuation, Ray& scattered)
    const override {
        scattered = Ray(rec.p, random_unit_vector());
        attenuation = tex->value(rec.u, rec.v, rec.p);
        return true;
    }

  private:
    std::shared_ptr<Texture> tex;
};
