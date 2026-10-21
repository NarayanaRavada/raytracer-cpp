#pragma once

#include "external/stb_image.h"

#include <algorithm>
#include <memory>
#include <string>

class Image {
public:
    explicit Image(const std::string& filename) {
        int channels{};

        const std::string path = std::string(ASSETS_DIR) + "/" + filename.c_str();

        data_.reset(
            stbi_load(path.c_str(), &width_, &height_, &channels, 3)
        );
    }

    [[nodiscard]] int width() const noexcept {
        return width_;
    }

    [[nodiscard]] int height() const noexcept {
        return height_;
    }

    [[nodiscard]] const unsigned char* pixel(int x, int y) const noexcept {
        static constexpr unsigned char magenta[] = {255, 0, 255};

        if (!data_ || width_ == 0 || height_ == 0)
            return magenta;

        x = std::clamp(x, 0, width_ - 1);
        y = std::clamp(y, 0, height_ - 1);

        return data_.get() + (y * width_ + x) * 3;
    }

    [[nodiscard]] bool valid() const noexcept {
        return data_ != nullptr;
    }

private:
    struct Deleter {
        void operator()(unsigned char* ptr) const noexcept {
            stbi_image_free(ptr);
        }
    };

    std::unique_ptr<unsigned char, Deleter> data_;
    int width_{};
    int height_{};
};
