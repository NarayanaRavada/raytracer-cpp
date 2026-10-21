#include "color.h"
#include "camera.h"
#include "bvh/bvh.h"
#include "hittable.h"
#include "vec3f.h"
#include "frame_buffer.h"
#include "hittable_list.h"
#include "textures/checker.h"
#include "textures/noise.h"
#include "textures/image_texture.h"

#include "materials/dielectric.h"
#include "materials/lambertian.h"
#include "materials/metal.h"
#include "materials/diffuse_light.h"

#include "objects/sphere.h"
#include "objects/quad.h"

#include "mediums/constant_medium.h"

#include "transforms/rotate.h"
#include "transforms/translate.h"

#include "export/save_as_ppm.hpp"

#include <memory>

int main()
{
    HittableList world;

    // Materials
    Material *ground = new Lambertian(Color(0.8, 0.8, 0.0));
    Material *blue = new Lambertian(Color(0.1, 0.2, 0.8));
    Material *glass = new Dielectric(3);
    Material *metal = new Metal(Color(0.8, 0.8, 0.8), 0.05);

    auto checker = std::make_shared<Checker>(0.1, Color(.2, .3, .1), Color(.9, .9, .9));
    Material* checkeredL = new Lambertian(checker);

    auto earthTexture = std::make_shared<ImageTexture>("earthmap.jpg");
    Material* earthL = new Lambertian(earthTexture);

    auto marsTexture = std::make_shared<ImageTexture>("mars.jpg");
    Material* marsL = new Lambertian(marsTexture);

    Camera cam;
    cam.aspect_ratio = 1.0f;
    cam.image_width = 600;
    cam.samples_per_pixel = 200;
    cam.max_depth = 50;


    auto textures = [&] () {
      // Ground
      world.add(std::make_unique<Sphere>(Point3f(0, -100.5, -1), 100, ground));
      // Mars
      world.add(std::make_unique<Sphere>(Point3f(-4, 0.5, -3), 0.9, marsL));
      // Checkered sphere
      world.add(std::make_unique<Sphere>(Point3f(-2.5, 0.5, -3), 0.5, checkeredL));
      // Glass sphere
      world.add(std::make_unique<Sphere>(Point3f(-1.2, 0.5, -3), 0.5, glass));
      // Blue sphere
      world.add(std::make_unique<Sphere>(Point3f(0.2, 0.5, -3), 0.5, blue));
      // Metal sphere
      world.add(std::make_unique<Sphere>(Point3f(1.5, 0.5, -3), 0.5, metal));
      // Earth
      world.add(std::make_unique<Sphere>(Point3f(3, 0.5, -3), 0.9, earthL));

      cam.vfov = 7.0f;
      cam.look_from = Point3f(5, 50, 15);
      cam.look_at = Point3f(0, 0.5, -3);
      cam.vup = Vec3f(0, 1, 0);
      cam.background = Color(0.70, 0.80, 1.00);

      cam.defocus_angle = 0.0f;
      cam.focus_distance = (cam.look_from - cam.look_at).length();
    };

    auto perlin = [&] {
      auto pertext = std::make_shared<Noise>(4);
      Material* pretextL = new Lambertian(pertext);
      world.add(std::make_unique<Sphere>(Point3f(0, -1000, 0), 1000, pretextL));

      world.add(std::make_unique<Sphere>(Point3f(0, 2, 0), 2, pretextL));

      cam.vfov     = 20;
      cam.look_from = Point3f(13,2,3);
      cam.look_at   = Point3f(0,0,0);
      cam.vup      = Vec3f(0,1,0);
      cam.background = Color(0.70, 0.80, 1.00);

      cam.defocus_angle = 0;
    };

    auto quads = [&]() {

      // Materials
      Material* leftRed     = new Lambertian(Color(1.0, 0.2, 0.2));
      Material* backGreen   = new Lambertian(Color(0.2, 1.0, 0.2));
      Material* rightBlue   = new Lambertian(Color(0.2, 0.2, 1.0));
      Material* upperOrange = new Lambertian(Color(1.0, 0.5, 0.0));
      Material* lowerTeal   = new Lambertian(Color(0.2, 0.8, 0.8));

      // Quads
      world.add(std::make_shared<Quad>(Point3f(-3,-2, 5), Vec3f(0, 0,-4), Vec3f(0, 4, 0), leftRed));
      world.add(std::make_shared<Quad>(Point3f(-2,-2, 0), Vec3f(4, 0, 0), Vec3f(0, 4, 0), backGreen));
      world.add(std::make_shared<Quad>(Point3f( 3,-2, 1), Vec3f(0, 0, 4), Vec3f(0, 4, 0), rightBlue));
      world.add(std::make_shared<Quad>(Point3f(-2, 3, 1), Vec3f(4, 0, 0), Vec3f(0, 0, 4), upperOrange));
      world.add(std::make_shared<Quad>(Point3f(-2,-3, 5), Vec3f(4, 0, 0), Vec3f(0, 0,-4), lowerTeal));

      cam.vfov     = 80;
      cam.look_from = Point3f(0,0,9);
      cam.look_at   = Point3f(0,0,0);
      cam.vup      = Vec3f(0,1,0);
      cam.background = Color(0.70, 0.80, 1.00);

      cam.defocus_angle = 0;
    };

    auto simple_light = [&]() {

      auto pertext = std::make_shared<Noise>(4);
      Material* pertextL = new Lambertian(pertext);

      world.add(
          std::make_shared<Sphere>(
            Point3f(0, 0, 0), 2, pertextL
            )
          );

      // Warm light — left
      Material* warmLight = new DiffuseLight(Color(4.0, 1.5, 0.5));

      world.add(
          std::make_shared<Quad>(
            Point3f(-4, 0, -1),
            Vec3f(0, 0, 2),
            Vec3f(0, 2, 0),
            warmLight
            )
          );

      // Cool light — right
      Material* coolLight = new DiffuseLight(Color(0.5, 1.5, 4.0));

      world.add(
          std::make_shared<Quad>(
            Point3f(4, 0, -1),
            Vec3f(0, 2, 0),
            Vec3f(0, 0, 2),
            coolLight
            )
          );

      cam.vfov = 30;
      cam.look_from = Point3f(0, 20, 0);
      cam.look_at = Point3f(0, 0, 0);
      cam.vup = Vec3f(0, 0, -1);

      cam.defocus_angle = 0;
    };

    auto cornell_box = [&] () {

      Material* red   = new Lambertian(Color(0.65, 0.05, 0.05));
      Material* white = new Lambertian(Color(0.73, 0.73, 0.73));
      Material* green = new Lambertian(Color(0.12, 0.45, 0.15));
      Material* light = new DiffuseLight(Color(15, 15, 15));

      world.add(std::make_shared<Quad>( Point3f(555, 0, 0), Vec3f(0, 555, 0), Vec3f(0, 0, 555), green));
      world.add(std::make_shared<Quad>( Point3f(0, 0, 0), Vec3f(0, 555, 0), Vec3f(0, 0, 555), red));
      world.add(std::make_shared<Quad>( Point3f(343, 554, 332), Vec3f(-130, 0, 0), Vec3f(0, 0, -105), light));
      world.add(std::make_shared<Quad>( Point3f(0, 0, 0), Vec3f(555, 0, 0), Vec3f(0, 0, 555), white));
      world.add(std::make_shared<Quad>( Point3f(555, 555, 555), Vec3f(-555, 0, 0), Vec3f(0, 0, -555), white));
      world.add(std::make_shared<Quad>( Point3f(0, 0, 555), Vec3f(555, 0, 0), Vec3f(0, 555, 0), white));

      std::shared_ptr<Hittable> box1 = box(Point3f(0,0,0), Point3f(165, 0, 0), Point3f(0, 330, 0), Point3f(0, 0, 165), white);
      box1 = make_shared<Rotate>(box1, Vec3f(0, 15, 0));
      box1 = make_shared<Translate>(box1, Vec3f(265,0,295));
      world.add(box1);

      std::shared_ptr<Hittable> box2 = box(Point3f(0,0,0), Point3f(165, 0, 0), Point3f(0, 165, 0), Point3f(0, 0, 165), white);
      box2 = make_shared<Rotate>(box2, Vec3f(0, -18, 0));
      box2 = make_shared<Translate>(box2, Vec3f(130,0,65));

      world.add(box2);

      cam.vfov       = 40;
      cam.look_from  = Point3f(278, 278, -800);
      cam.look_at    = Point3f(278, 278, 0);
      cam.vup        = Vec3f(0, 1, 0);
      cam.defocus_angle = 0;
    };

    auto cornell_smoke = [&] () {
      auto red   = std::make_shared<Lambertian>(Color(0.65, 0.05, 0.05));
      auto white = std::make_shared<Lambertian>(Color(0.73, 0.73, 0.73));
      auto green = std::make_shared<Lambertian>(Color(0.12, 0.45, 0.15));
      auto light = std::make_shared<DiffuseLight>(Color(7, 7, 7));

      world.add(std::make_shared<Quad>( Point3f(555, 0, 0), Vec3f(0, 555, 0), Vec3f(0, 0, 555), green.get()));
      world.add(std::make_shared<Quad>( Point3f(0, 0, 0), Vec3f(0, 555, 0), Vec3f(0, 0, 555), red.get()));
      world.add(std::make_shared<Quad>( Point3f(113, 554, 127), Vec3f(330, 0, 0), Vec3f(0, 0, 305), light.get()));
      world.add(std::make_shared<Quad>( Point3f(0, 555, 0), Vec3f(555, 0, 0), Vec3f(0, 0, 555), white.get()));
      world.add(std::make_shared<Quad>( Point3f(0, 0, 0), Vec3f(555, 0, 0), Vec3f(0, 0, 555), white.get()));
      world.add(std::make_shared<Quad>( Point3f(0, 0, 555), Vec3f(555, 0, 0), Vec3f(0, 555, 0), white.get()));

      std::shared_ptr<Hittable> box1 = box(
          Point3f(0, 0, 0),
          Vec3f(165, 0, 0),
          Vec3f(0, 330, 0),
          Vec3f(0, 0, 165),
          white.get()
          );
      box1 = std::make_shared<Rotate>(box1, Vec3f(0, 15, 0));
      box1 = std::make_shared<Translate>(box1, Vec3f(265, 0, 295));
      world.add(std::make_shared<ConstantMedium>(box1, 0.01, Color(0, 0, 0)));

      std::shared_ptr<Hittable> box2 = box(
          Point3f(0, 0, 0),
          Vec3f(165, 0, 0),
          Vec3f(0, 165, 0),
          Vec3f(0, 0, 165),
          white.get()
          );
      box2 = std::make_shared<Rotate>(box2, Vec3f(0, -18, 0));
      box2 = std::make_shared<Translate>(box2, Vec3f(130, 0, 65));

      world.add(std::make_shared<ConstantMedium>(box2, 0.01, Color(1, 1, 1)));

      cam.vfov     = 40;
      cam.look_from = Point3f(278, 278, -800);
      cam.look_at   = Point3f(278, 278, 0);
      cam.vup      = Vec3f(0, 1, 0);

      cam.defocus_angle = 0;
    };

    auto final_scene = [&](int imageWidth, int samplesPerPixel, int maxDepth)  {
      HittableList boxes1;

      Material* ground = new Lambertian(Color(0.48, 0.83, 0.53));

      constexpr int boxesPerSide = 20;

      for (int i = 0; i < boxesPerSide; i++) {
        for (int j = 0; j < boxesPerSide; j++) {
          constexpr double w = 100.0;

          const double x0 = -1000.0 + i * w;
          const double z0 = -1000.0 + j * w;
          const double y0 = 0.0;

          const double x1 = x0 + w;
          const double y1 = random_float(1, 101);
          const double z1 = z0 + w;

          boxes1.add(box(
                Point3f(x0, y0, z0),
                Point3f(x1, 0, 0),
                Point3f(0, y1, 0),
                Point3f(0, 0, z1),
                ground
                ));
        }
      }

      world.add(std::make_shared<BVHNode>(boxes1));

      // Area light
      Material* light = new DiffuseLight(Color(7, 7, 7));

      world.add(std::make_shared<Quad>(
            Point3f(123, 554, 147),
            Vec3f(300, 0, 0),
            Vec3f(0, 0, 265),
            light
            ));

      // Moving sphere
      const Point3f center1(400, 400, 200);
      const Point3f center2 = center1 + Vec3f(30, 0, 0);

      Material* sphereMaterial = new Lambertian(Color(0.7, 0.3, 0.1));

      world.add(std::make_shared<Sphere>(center1, 50, sphereMaterial));

      // Glass sphere
      world.add(std::make_shared<Sphere>(Point3f(260, 150, 45), 50, new Dielectric(1.5)));

      // Metal sphere
      world.add(std::make_shared<Sphere>(Point3f(0, 150, 145), 50, new Metal(Color(0.8, 0.8, 0.9), 1.0)));

      // Blue participating medium
      auto boundary = std::make_shared<Sphere>(Point3f(360, 150, 145), 70, new Dielectric(1.5));

      world.add(boundary);

      world.add(std::make_shared<ConstantMedium>(boundary, 0.2, Color(0.2, 0.4, 0.9)));

      // Atmospheric medium
      boundary = std::make_shared<Sphere>(Point3f(0, 0, 0), 5000, new Dielectric(1.5));

      world.add(std::make_shared<ConstantMedium>(boundary, 0.0001, Color(1, 1, 1)));

      // Earth texture
      auto earthTexture = std::make_shared<ImageTexture>("earthmap.jpg");
      Material* earthMaterial = new Lambertian(earthTexture);

      world.add(std::make_shared<Sphere>(
            Point3f(400, 200, 400),
            100,
            earthMaterial
            ));

      // Noise-textured sphere
      auto noiseTexture = std::make_shared<Noise>(0.2);
      Material* noiseMaterial = new Lambertian(noiseTexture);

      world.add(std::make_shared<Sphere>(
            Point3f(220, 280, 300),
            80,
            noiseMaterial
            ));

      // Cluster of small spheres
      HittableList boxes2;

      Material* white = new Lambertian(Color(0.73, 0.73, 0.73));

      constexpr int sphereCount = 1000;

      for (int j = 0; j < sphereCount; j++) {
        boxes2.add(std::make_shared<Sphere>(
              Point3f::random(0, 165),
              10,
              white
              ));
      }

      std::shared_ptr<Hittable> cluster =
        std::make_shared<BVHNode>(boxes2);

      cluster = std::make_shared<Rotate>(
          cluster,
          Vec3f(0, 15, 0)
          );

      cluster = std::make_shared<Translate>(
          cluster,
          Vec3f(-100, 270, 395)
          );

      world.add(cluster);

      cam.background = Color(0, 0, 0);

      cam.image_width = imageWidth;
      cam.samples_per_pixel = samplesPerPixel;
      cam.max_depth = maxDepth;

      cam.vfov = 40;
      cam.look_from = Point3f(478, 278, -600);
      cam.look_at = Point3f(278, 278, 0);
      cam.vup = Vec3f(0, 1, 0);

      cam.defocus_angle = 0;
    };

    /* camera settings */
    switch (0) {
      case 1:
        textures(); break;
      case 2:
        perlin(); break;
      case 3:
        quads(); break;
      case 4:
        simple_light(); break;
      case 5:
        cornell_box(); break;
      case 6:
        cornell_smoke();
      case 7:
        final_scene(800, 10000, 40);
        break;
      default:
        final_scene(400, 50, 40);
        break;
    }

    cam.initialize();

    BVHNode bvh(world);

    FrameBuffer buf(cam.image_width, cam.image_height);
    cam.render(bvh, buf);

    save_as_ppm(buf, "output.ppm");

    return 0;
}
