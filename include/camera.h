#pragma once

#include "color.h"
#include "frame_buffer.h"
#include "hittable.h"
#include "material.h"
#include "ray.h"
#include "utils.h"
#include "vec3f.h"

#include <algorithm>
#include <atomic>
#include <chrono>
#include <cmath>
#include <iostream>
#include <thread>
#include <vector>

class Camera {
public:
    Point3f center;

    float aspect_ratio = 1.0f;
    int image_width = 800;
    int image_height;

    int samples_per_pixel = 100;
    int max_depth = 50;

    float vfov = 90.0f;

    Vec3f look_from = Point3f(0, 0, 0);
    Vec3f look_at = Point3f(0, 0, -1);
    Vec3f vup = Vec3f(0, 1, 0);

    float defocus_angle = 0.0f;
    float focus_distance = 10.0f;

    Color background = Color(0, 0, 0);

    void initialize() {
        image_height = std::max(
            1,
            static_cast<int>(image_width / aspect_ratio)
        );

        center = look_from;

        const auto theta = degrees_to_radians(vfov);
        const auto h = std::tan(theta / 2.0f);

        const auto viewport_height = 2.0f * h * focus_distance;
        const auto viewport_width =
            viewport_height *
            (static_cast<float>(image_width) / image_height);

        w = unit_vector(look_from - look_at);
        u = unit_vector(cross(vup, w));
        v = cross(w, u);

        const auto viewport_u = viewport_width * u;
        const auto viewport_v = -viewport_height * v;

        pixel_du = viewport_u / static_cast<float>(image_width);
        pixel_dv = viewport_v / static_cast<float>(image_height);

        const auto viewport_00 =
            center
            - focus_distance * w
            - viewport_u / 2.0f
            - viewport_v / 2.0f;

        pixel00_loc = viewport_00 + (pixel_du + pixel_dv) / 2.0f;

        const auto defocus_radius =
            focus_distance *
            std::tan(degrees_to_radians(defocus_angle / 2.0f));

        defocus_disk_u = u * defocus_radius;
        defocus_disk_v = v * defocus_radius;
    }

    void render(const Hittable& world, FrameBuffer& displayBuffer) {
        constexpr int TILE_SIZE = 16;

        const int pixel_count = image_width * image_height;

        std::vector<Color> frame_buffer(pixel_count);

        const int tile_rows =
            (image_height + TILE_SIZE - 1) / TILE_SIZE;

        const int tile_cols =
            (image_width + TILE_SIZE - 1) / TILE_SIZE;

        const int total_tiles = tile_rows * tile_cols;

        std::atomic<int> next_tile{0};

        // Each tile has exactly one writer.
        std::vector<std::atomic<bool>> tile_done(total_tiles);

        for (auto& done : tile_done) {
            done.store(false, std::memory_order_relaxed);
        }

        const auto idx = [this](int x, int y) {
            return y * image_width + x;
        };

        auto render_tile = [&](int tile) {
            const int tile_x = tile % tile_cols;
            const int tile_y = tile / tile_cols;

            const int x0 = tile_x * TILE_SIZE;
            const int y0 = tile_y * TILE_SIZE;

            const int x1 = std::min(x0 + TILE_SIZE, image_width);
            const int y1 = std::min(y0 + TILE_SIZE, image_height);

            for (int y = y0; y < y1; ++y) {
                for (int x = x0; x < x1; ++x) {
                    Color pixel_color(0, 0, 0);

                    for (int sample = 0;
                         sample < samples_per_pixel;
                         ++sample) {

                        const Ray ray = get_ray(x, y);

                        pixel_color += ray_color(
                            ray,
                            max_depth,
                            world
                        );
                    }

                    pixel_color /= static_cast<float>(samples_per_pixel);

                    frame_buffer[idx(x, y)] = pixel_color;
                }
            }

            // Publish only after every pixel in the tile is written.
            tile_done[tile].store(true, std::memory_order_release);
        };

        auto worker = [&]() {
            while (true) {
                const int tile =
                    next_tile.fetch_add(1, std::memory_order_relaxed);

                if (tile >= total_tiles) {
                    break;
                }

                render_tile(tile);
            }
        };

        const auto hardware_threads =
            std::max(1u, std::thread::hardware_concurrency());

        // Leave one core for the display/event loop when possible.
        const unsigned thread_count =
            hardware_threads > 1 ? hardware_threads - 1 : 1;

        std::vector<std::thread> threads;
        threads.reserve(thread_count);

        const auto start = std::chrono::steady_clock::now();

        for (unsigned i = 0; i < thread_count; ++i) {
            threads.emplace_back(worker);
        }

        // Only the main thread accesses the display buffer.
        std::vector<bool> displayed(total_tiles, false);

        int displayed_count = 0;

        while (displayed_count < total_tiles) {
            for (int tile = 0; tile < total_tiles; ++tile) {
                if (displayed[tile]) {
                    continue;
                }

                if (!tile_done[tile].load(std::memory_order_acquire)) {
                    continue;
                }

                const int tile_x = tile % tile_cols;
                const int tile_y = tile / tile_cols;

                const int x0 = tile_x * TILE_SIZE;
                const int y0 = tile_y * TILE_SIZE;

                const int x1 = std::min(x0 + TILE_SIZE, image_width);
                const int y1 = std::min(y0 + TILE_SIZE, image_height);

                for (int y = y0; y < y1; ++y) {
                    for (int x = x0; x < x1; ++x) {
                        displayBuffer.at(x, y) = frame_buffer[idx(x, y)];
                    }
                }

                displayed[tile] = true;
                ++displayed_count;
            }

            if (displayed_count < total_tiles) {
                std::this_thread::sleep_for(
                    std::chrono::milliseconds(16)
                );
            }
        }

        for (auto& thread : threads) {
            thread.join();
        }

        const auto elapsed =
            std::chrono::duration_cast<std::chrono::microseconds>(
                std::chrono::steady_clock::now() - start
            ).count();

        std::clog
            << "\nRender time: "
            << elapsed / 1000.0
            << " ms\n";
    }

    int getImageHeight() const {
        return image_height;
    }

    int getImageWidth() const {
        return image_width;
    }

private:
    Ray get_ray(int x, int y) {
        const auto offset = random_square();

        const auto pixel_pos =
            pixel00_loc
            + (x + offset.x) * pixel_du
            + (y + offset.y) * pixel_dv;

        auto ray_origin = center;

        if (defocus_angle > 0.0f) {
            const auto p = random_in_unit_disk();

            ray_origin =
                center
                + p.x * defocus_disk_u
                + p.y * defocus_disk_v;
        }

        return Ray(ray_origin, pixel_pos - ray_origin);
    }

    Color ray_color(const Ray& ray, int depth, const Hittable& world) {
        if (depth <= 0) {
            return Color(0, 0, 0);
        }

        HitRecord rec;

        if (!world.hit(ray, Interval(0.001, INFINITY_), rec)) {
            return background;
        }

        const Color emitted =
            rec.material->emitted(rec.u, rec.v, rec.p);

        Ray scattered;
        Color attenuation;

        if (!rec.material->scatter(
                ray,
                rec,
                attenuation,
                scattered)) {
            return emitted;
        }

        return emitted
            + attenuation * ray_color(scattered, depth - 1, world);
    }

    Point3f pixel00_loc;
    Point3f pixel_du;
    Point3f pixel_dv;

    Vec3f u;
    Vec3f v;
    Vec3f w;

    Vec3f defocus_disk_u;
    Vec3f defocus_disk_v;
};
