#pragma once

#include <cstddef>
#include <stdexcept>
#include <vector>

#include "color.h"

struct FrameBuffer {
    int width;
    int height;
    std::vector<Color> pixels;

    FrameBuffer(int width, int height)
        : width(width),
          height(height),
          pixels(static_cast<std::size_t>(width) * height, Color(0, 0, 0))
    {
        if (width <= 0 || height <= 0) {
            throw std::invalid_argument("Frame dimensions must be positive");
        }
    }

    Color& at(int x, int y) {
        return pixels[static_cast<std::size_t>(y) * width + x];
    }

    const Color& at(int x, int y) const {
        return pixels[static_cast<std::size_t>(y) * width + x];
    }
};
