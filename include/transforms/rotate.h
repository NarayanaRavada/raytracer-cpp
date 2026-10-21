#pragma once

#include <memory>
#include <cmath>

#include "hittable.h"

class Rotate : public Hittable
{
public:
    // Angles are in degrees: (X, Y, Z).
    Rotate(std::shared_ptr<Hittable> object, const Vec3f& angles)
        : object_(std::move(object))
    {
        const double rx = degrees_to_radians(angles.x);
        const double ry = degrees_to_radians(angles.y);
        const double rz = degrees_to_radians(angles.z);

        const double cx = std::cos(rx);
        const double sx = std::sin(rx);
        const double cy = std::cos(ry);
        const double sy = std::sin(ry);
        const double cz = std::cos(rz);
        const double sz = std::sin(rz);

        // Combined rotation matrix: R = Rz * Ry * Rx.
        rotation_ = {{
            {cz * cy, cz * sy * sx - sz * cx, cz * sy * cx + sz * sx},
            {sz * cy, sz * sy * sx + cz * cx, sz * sy * cx - cz * sx},
            {-sy,     cy * sx,                cy * cx}
        }};

        // The inverse of a rotation matrix is its transpose.
        inverse_ = {{
            {rotation_[0][0], rotation_[1][0], rotation_[2][0]},
            {rotation_[0][1], rotation_[1][1], rotation_[2][1]},
            {rotation_[0][2], rotation_[1][2], rotation_[2][2]}
        }};

        // Rotate all eight bounding-box corners to construct
        // a world-space AABB enclosing the rotated object.
        const AABB original = object_->bounding_box();

        const Point3f corners[8] = {
            Point3f(original.x.min, original.y.min, original.z.min),
            Point3f(original.x.min, original.y.min, original.z.max),
            Point3f(original.x.min, original.y.max, original.z.min),
            Point3f(original.x.min, original.y.max, original.z.max),
            Point3f(original.x.max, original.y.min, original.z.min),
            Point3f(original.x.max, original.y.min, original.z.max),
            Point3f(original.x.max, original.y.max, original.z.min),
            Point3f(original.x.max, original.y.max, original.z.max)
        };

        Point3f min(INFINITY_, INFINITY_, INFINITY_);
        Point3f max(-INFINITY_, -INFINITY_, -INFINITY_);

        for (const auto& corner : corners) {
            const auto p = rotate_point(corner, rotation_);

            min.x = std::fmin(min.x, p.x);
            min.y = std::fmin(min.y, p.y);
            min.z = std::fmin(min.z, p.z);

            max.x = std::fmax(max.x, p.x);
            max.y = std::fmax(max.y, p.y);
            max.z = std::fmax(max.z, p.z);
        }

        bbox_ = AABB(min, max);
    }

    bool hit(
        const Ray& r,
        Interval ray_t,
        HitRecord& rec) const override
    {
        // Transform the ray into object space using the inverse rotation.
        const Point3f origin = rotate_point(r.origin(), inverse_);
        const Vec3f direction = rotate_vector(r.direction(), inverse_);

        Ray rotated_ray(origin, direction);

        if (!object_->hit(rotated_ray, ray_t, rec))
            return false;

        // Transform the intersection back into world space.
        rec.p = rotate_point(rec.p, rotation_);
        rec.normal = unit_vector(rotate_vector(rec.normal, rotation_));

        return true;
    }

    AABB bounding_box() const override
    {
        return bbox_;
    }

private:
    using Mat3 = std::array<std::array<double, 3>, 3>;

    static double degrees_to_radians(double degrees)
    {
        return degrees * std::numbers::pi / 180.0;
    }

    static Vec3f rotate_vector(const Vec3f& v, const Mat3& m)
    {
        return Vec3f(
            m[0][0] * v.x + m[0][1] * v.y + m[0][2] * v.z,
            m[1][0] * v.x + m[1][1] * v.y + m[1][2] * v.z,
            m[2][0] * v.x + m[2][1] * v.y + m[2][2] * v.z
        );
    }

    static Point3f rotate_point(const Point3f& p, const Mat3& m)
    {
        const auto v = rotate_vector(Vec3f(p.x, p.y, p.z), m);
        return Point3f(v.x, v.y, v.z);
    }

    std::shared_ptr<Hittable> object_;
    Mat3 rotation_{};
    Mat3 inverse_{};
    AABB bbox_;
};
