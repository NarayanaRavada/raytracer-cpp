#pragma once

#include "interval.h"
#include "vec3f.h"

#include <cmath>
#include <cstdint>
#include <iostream>
#include <ostream>

using Color = Vec3f;

#pragma once

#include "interval.h"
#include "vec3f.h"

#include <cmath>
#include <cstdint>
#include <ostream>

using Color = Vec3f;

inline float linear_to_gamma(float linear_component) {
    if (linear_component > 0.0f) {
        return std::sqrt(linear_component);
    }

    return 0.0f;
}

// Reinhard tone mapping
inline float reinhard_tone_map(float value) {
    value = std::fmax(0.0f, value);
    return value / (1.0f + value);
}

inline void write_color(std::ostream& f, const Color& c) {
    static const Interval intensity(0.0f, 1.0f);

    // 1. Tone map linear radiance
    Color mapped = map(c, reinhard_tone_map);

    // 2. Apply gamma correction
    Color gamma_corrected = map(mapped, linear_to_gamma);

    // 3. Convert to 8-bit RGB
    const uint8_t rgb[3] = {
        static_cast<uint8_t>(255.999f * intensity.clamp(gamma_corrected.x)),
        static_cast<uint8_t>(255.999f * intensity.clamp(gamma_corrected.y)),
        static_cast<uint8_t>(255.999f * intensity.clamp(gamma_corrected.z))
    };

    f.write(reinterpret_cast<const char*>(rgb), 3);
}
