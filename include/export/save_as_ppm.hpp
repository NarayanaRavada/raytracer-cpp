#include <fstream>
#include <stdexcept>

#include "frame_buffer.h"

#pragma once

#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>

#include "../frame_buffer.h"
#include "../color.h"

inline void save_as_ppm(const FrameBuffer& frame, const std::string& filename)
{
    std::ofstream out(filename, std::ios::binary);

    if (!out.is_open()) {
        throw std::runtime_error("Could not open: " + filename);
    }

    std::cout << "Framebuffer: "
              << frame.width << "x" << frame.height
              << ", pixels: " << frame.pixels.size() << '\n';

    out << "P6\n"
        << frame.width << ' ' << frame.height << '\n'
        << "255\n";

    for (const Color& pixel : frame.pixels) {
        Color c = pixel;
        write_color(out, c);
    }

    out.flush();

    if (!out.good()) {
        throw std::runtime_error("Failed while writing: " + filename);
    }

    std::cout << "Wrote PPM: " << filename << ", bytes: " << out.tellp() << '\n';
}
