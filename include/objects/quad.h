#pragma once

#include "bvh/aabb.h"
#include "hittable.h"
#include "hittable_list.h"
#include "interval.h"
#include "material.h"
#include "vec3f.h"
#include <memory>

class Quad: public Hittable {
public:
  Quad(const Point3f &Q, const Point3f &u, const Point3f &v, const Material* mat)
    : Q_(Q), u_(u), v_(v) {
    auto n = cross(u, v);
    normal_ = unit_vector(n);
    D_ = dot(normal_, Q);
    w_ = n / dot(n, n);
    material_ = mat;
  }

  bool hit(const Ray &r, Interval rayT, HitRecord &rec) const override {
    auto denom = dot(normal_, r.direction());

    if (std::fabs(denom) < 1e-8)
      return false;

    auto t = (D_ - dot(normal_, r.origin())) / denom;
    if (!rayT.contains(t))
      return false;

    auto intersection = r.at(t);
    Vec3f planarHit = intersection - Q_;
    auto alpha = dot(w_, cross(planarHit, v_));
    auto beta = dot(w_, cross(u_, planarHit));

    if (!is_interior(alpha, beta, rec))
      return false;

    rec.t = t;
    rec.p = intersection;
    rec.material = material_;
    rec.set_face_normal(r, normal_);

    return true;
  }

  virtual bool is_interior(double a, double b, HitRecord& rec) const {
    Interval unitVector = Interval(0, 1);

    if (!unitVector.contains(a) || !unitVector.contains(b))
      return false;

    rec.u = a;
    rec.v = b;
    return true;
  }

  AABB bounding_box() const override { return bbox_; }

private:
  Point3f Q_;
  Vec3f u_, v_;
  Vec3f w_;
  Vec3f normal_;
  const Material *material_;
  AABB bbox_;
  double D_;
};

inline std::shared_ptr<HittableList> box(
    const Point3f& p,
    const Vec3f& u,
    const Vec3f& v,
    const Vec3f& w,
    const Material* mat)
{
    auto sides = std::make_shared<HittableList>();

    // Six faces.
    sides->add(std::make_shared<Quad>(p,           u,          v, mat)); // Front
    sides->add(std::make_shared<Quad>(p + u,       w,          v, mat)); // Right
    sides->add(std::make_shared<Quad>(p + u + w,  -1.0 * u,   v, mat)); // Back
    sides->add(std::make_shared<Quad>(p + w,      -1.0 * w,   v, mat)); // Left
    sides->add(std::make_shared<Quad>(p + v,       u,          w, mat)); // Top
    sides->add(std::make_shared<Quad>(p,           u,          w, mat)); // Bottom

    return sides;
}
