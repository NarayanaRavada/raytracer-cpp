#include "color.h"
#include "material.h"
#include "ray.h"
#include "utils.h"
#include "vec3f.h"

class Dielectric : public Material {
public:
    explicit Dielectric(float relative_refractive_index)
        : refraction_index_(relative_refractive_index) {}

    bool scatter(
        const Ray& r,
        const HitRecord& rec,
        Color& attenuation,
        Ray& scattered
    ) const override {
        attenuation = Color(1.0, 1.0, 1.0);

        const float ri = rec.frontFace
            ? (1.0f / refraction_index_)
            : refraction_index_;

        const Vec3f unit_direction = unit_vector(r.direction());

        const float cos_theta = std::fmin(
            dot(-1.0f * unit_direction, rec.normal), 1.0f
        );

        const float sin_theta = std::sqrt(1.0f - cos_theta * cos_theta);

        const bool cannot_refract = ri * sin_theta > 1.0f;

        Vec3f direction;

        if (cannot_refract || reflectance(cos_theta, ri) > random_float()) {
            direction = reflect(unit_direction, rec.normal);
        } else {
            direction = refract(unit_direction, rec.normal, ri);
        }

        scattered = Ray(rec.p, direction);
        return true;
    }

private:
    float refraction_index_;

    static float reflectance(float cosine, float refraction_index) {
        const float r0 = std::pow(
            (1.0f - refraction_index) / (1.0f + refraction_index), 2.0f
        );

        return r0 + (1.0f - r0) * std::pow(1.0f - cosine, 5.0f);
    }
};
