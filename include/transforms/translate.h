#pragma once

#include "bvh/aabb.h"
#include "hittable.h"
#include "interval.h"
#include "ray.h"
#include "vec3f.h"
#include <memory>

class Translate: public Hittable {
public:
  Translate(std::shared_ptr<Hittable> object, const Vec3f &offset)
    : object_(object), offset_(offset)
    {
      bbox_ = object_->bounding_box() + offset_;
    }

  bool hit(const Ray &r, Interval rayT, HitRecord &rec) const override {
    Ray offsetR(r.origin() - offset_, r.direction());

    if (!object_->hit(offsetR, rayT, rec))
      return false;

    rec.p += offset_;

    return true;
  }

  AABB bounding_box() const override { return bbox_; }

private:
  std::shared_ptr<Hittable> object_;
  Vec3f offset_;
  AABB bbox_;
};
