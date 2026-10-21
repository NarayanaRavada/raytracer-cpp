#pragma once

#include "bvh/aabb.h"
#include "constants.h"
#include "hittable.h"
#include "interval.h"
#include "vec3f.h"

#include <algorithm>
#include <cassert>
#include <cmath>

class Sphere : public Hittable {
public:
  Sphere(const Point3f &center, float radius, const Material* material)
    : center_(center), radius_(radius), material_(std::move(material))
  {
    auto rvec = Vec3f(radius_);
    bbox_ = AABB(center_ - rvec, center_ + rvec);
  };

  bool hit(const Ray &r, Interval rayT, HitRecord &rec) const override {
    Vec3f oc = center_ - r.origin();
    auto a = r.direction().length_squared();
    auto h = dot(r.direction(), oc);
    auto c = oc.length_squared() - radius_ * radius_;
    auto discriminant = h * h - a * c;

    if (discriminant <= 0) {
      return false;
    }

    auto sqrtD = std::sqrt(discriminant);

    auto root = (h - sqrtD) / a;
    if (!rayT.contains(root)) {
      root = (h + sqrtD) / a;
      if (!rayT.contains(root)) {
        return false;
      }
    }

    rec.t = root;
    rec.p = r.at(rec.t);
    auto outwardNormal = (rec.p - center_) / radius_;
    rec.set_face_normal(r, outwardNormal);
    getSphereUV(outwardNormal, rec.u, rec.v);
    rec.material = material_;

    return true;
  }

  AABB bounding_box() const override { return bbox_; }

private:

  static void getSphereUV(const Point3f& p, double& u, double& v) {
    float y = std::clamp(p.y, -1.0f, 1.0f);
    auto theta = std::acos(-y);
    auto phi = std::atan2(-p.z, p.x) + PI_;

    u = phi / (2 * PI_);
    v = theta / PI_;
  }

  Point3f center_;
  float radius_;
  const Material* material_;
  AABB bbox_;
};
