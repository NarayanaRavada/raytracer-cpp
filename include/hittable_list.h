#pragma once

#include "bvh/aabb.h"
#include "hittable.h"
#include "interval.h"
#include <memory>
#include <vector>

class HittableList : public Hittable {
public:
  std::vector<std::shared_ptr<Hittable>> objects_;

  HittableList() {};
  HittableList(std::shared_ptr<Hittable> object) { add(std::move(object)); };

  void add(std::shared_ptr<Hittable> object) {
    objects_.push_back(object);
    bbox_ = AABB(bbox_, object->bounding_box());
  }

  bool hit(const Ray &r, Interval rayT, HitRecord &rec) const override {
    HitRecord Trec;
    bool didHit = false;
    auto closest = rayT.max;

    for (const auto &object : objects_) {
      if (object->hit(r, Interval(rayT.min, closest), Trec)) {
        didHit = true;
        closest = Trec.t;
        rec = Trec;
      }
    }

    return didHit;
  }

  AABB bounding_box() const override { return bbox_; }

private:
  AABB bbox_;
};
