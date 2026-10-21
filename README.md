# C++ Path Tracer

A CPU-based path tracer built from scratch in C++20, focused on realistic rendering and performance.

### Features
- BVH acceleration for ray intersections
- Multithreaded tile-based rendering
- Materials, textures, and glass
- Volumetric effects and participating media
- Physically based light transport

### Build

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j$(nproc)
```
