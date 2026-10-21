#pragma once

#include "bvh/aabb.h"
#include "hittable.h"
#include "hittable_list.h"
#include "interval.h"
#include "ray.h"
#include "utils.h"
#include <algorithm>
#include <iterator>
#include <memory>

class BVHNode: public Hittable  {
public:
  BVHNode(HittableList list): BVHNode(list.objects_, 0, list.objects_.size()) { }

  BVHNode(std::vector<std::shared_ptr<Hittable>>& objects, size_t start, size_t end) {
    int axis = random_int(0, 2);

    auto comparator = (axis == 0) ? box_x_compare
                    : (axis == 1) ? box_y_compare
                                  : box_z_compare;

    size_t objectSpan = end - start;

    if (objectSpan == 1) {
      left = right = objects[start];
    } else if (objectSpan == 2) {
      left = objects[start];
      right = objects[start + 1];
    } else {
      std::sort(std::begin(objects) + start, std::begin(objects) + end, comparator);

      auto mid = start + objectSpan/2;
      left = std::make_shared<BVHNode>(objects, start, mid);
      right = std::make_shared<BVHNode>(objects, mid, end);
    }

    bbox_ = AABB(left->bounding_box(), right->bounding_box());
  }

  bool hit(const Ray& r, Interval rayT, HitRecord& rec) const override {
    if (!bbox_.hit(r, rayT))
      return false;

    bool hitLeft = left->hit(r, rayT, rec);
    bool hitRight = right->hit(r, Interval(rayT.min, hitLeft ? rec.t : rayT.max), rec);

    return hitLeft || hitRight;
  }

  AABB bounding_box() const override { return bbox_; }

private:
  AABB bbox_;
  std::shared_ptr<Hittable> left;
  std::shared_ptr<Hittable> right;

  static bool box_compare(const std::shared_ptr<Hittable> a, const std::shared_ptr<Hittable> b, int axis_index) {
    auto a_axis_interval = a->bounding_box().axis_interval(axis_index);
    auto b_axis_interval = b->bounding_box().axis_interval(axis_index);
    return a_axis_interval.min < b_axis_interval.min;
  }

  static bool box_x_compare (const std::shared_ptr<Hittable> a, const std::shared_ptr<Hittable> b) {
    return box_compare(a, b, 0);
  }

  static bool box_y_compare (const std::shared_ptr<Hittable> a, const std::shared_ptr<Hittable> b) {
    return box_compare(a, b, 1);
  }

  static bool box_z_compare (const std::shared_ptr<Hittable> a, const std::shared_ptr<Hittable> b) {
    return box_compare(a, b, 2);
  }
};
