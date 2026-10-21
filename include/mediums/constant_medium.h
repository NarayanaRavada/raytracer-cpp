#include "constants.h"
#include "hittable.h"
#include "interval.h"
#include "material.h"
#include "texture.h"

#include "textures/isotropic.h"

#include <cmath>
#include <memory>
#include <utility>

class ConstantMedium : public Hittable {
  public:
    ConstantMedium(std::shared_ptr<Hittable> boundary, double density, std::shared_ptr<Texture> tex)
      : boundary_(boundary), negInvDensity_(-1/density),
        phaseFunctionOwner_(std::make_shared<Isotropic>(std::move(tex))),
        phaseFunction_(phaseFunctionOwner_.get())
    {}

    ConstantMedium(std::shared_ptr<Hittable> boundary, double density, const Color& albedo)
      : boundary_(boundary), negInvDensity_(-1/density),
        phaseFunctionOwner_(std::make_shared<Isotropic>(albedo)),
        phaseFunction_(phaseFunctionOwner_.get())
    {}

    bool hit(const Ray& r, Interval rayT, HitRecord& rec) const override {
        HitRecord rec1, rec2;

        if (!boundary_->hit(r, INFINITE_INTERVAL_, rec1))
            return false;

        if (!boundary_->hit(r, Interval(rec1.t+0.0001, INFINITY_), rec2))
            return false;

        if (rec1.t < rayT.min) rec1.t = rayT.min;
        if (rec2.t > rayT.max) rec2.t = rayT.max;

        if (rec1.t >= rec2.t)
            return false;

        if (rec1.t < 0)
            rec1.t = 0;

        auto ray_length = r.direction().length();
        auto distance_inside_boundary = (rec2.t - rec1.t) * ray_length;
        auto hit_distance = negInvDensity_ * std::log(random_float());

        if (hit_distance > distance_inside_boundary)
            return false;

        rec.t = rec1.t + hit_distance / ray_length;
        rec.p = r.at(rec.t);

        rec.normal = Vec3f(1,0,0);  // arbitrary
        rec.frontFace = true;     // also arbitrary
        rec.material = phaseFunction_;

        return true;
    }

    AABB bounding_box() const override { return boundary_->bounding_box(); }

  private:
    std::shared_ptr<Hittable> boundary_;
    double negInvDensity_;
    std::shared_ptr<Material> phaseFunctionOwner_;
    const Material* phaseFunction_;
};
