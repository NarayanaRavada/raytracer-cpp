#pragma once

#include "bvh/aabb.h"
#include "interval.h"
#include "ray.h"
#include "vec3f.h"

class Material;

class HitRecord {
public:
  Point3f p;
  Vec3f normal;
  const Material* material = nullptr;
  float t;
  double u, v;
  bool frontFace;

  void set_face_normal(const Ray &r, const Vec3f &outwardNormal) {
    frontFace = dot(r.direction(), outwardNormal) < 0;
    normal = frontFace ? outwardNormal : -1 * outwardNormal;
  }
};

class Hittable {
public:
  virtual ~Hittable() = default;

  virtual bool hit(const Ray &r, Interval rayT, HitRecord &rec) const = 0;

  virtual AABB bounding_box() const = 0;
};
