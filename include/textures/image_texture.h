#pragma once

#include "image.h"
#include "texture.h"

#include <algorithm>
#include <string>

class ImageTexture : public Texture {
public:
    explicit ImageTexture(const std::string& filename)
        : image_(filename) {}

    Color value(double u, double v, const Point3f&) const override {
        if (!image_.valid())
            return Color(0.0f, 1.0f, 1.0f);

        u = std::clamp(u, 0.0, 1.0);
        v = std::clamp(v, 0.0, 1.0);

        int x = static_cast<int>(u * image_.width());
        int y = static_cast<int>((1.0f - v) * image_.height());

        const auto* pixel = image_.pixel(x, y);
        constexpr float inv255 = 1.0f / 255.0f;

        return Color(
            pixel[0] * inv255,
            pixel[1] * inv255,
            pixel[2] * inv255
        );
    }

private:
    Image image_;
};
