#include "Header_Files/color.h"
#include "Header_Files/vec3.h"
#include "Header_Files/constants.h"
#include "Header_Files/sphere.h"
#include "Header_Files/entity.h"
#include "Header_Files/entity_list.h"
#include "Header_Files/ray.h"
#include "Header_Files/camera.h"
#include "Header_Files/material.h"
#include "Header_Files/pyramid.h"
#include <iostream>

using namespace std;
int main() {
    entity_list world;

    auto ground_material = make_shared<lambertian>(color(0.5, 0.5, 0.5));
    world.add(make_shared<sphere>(point3(0,-1000,0), 1000, ground_material));

    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            auto choose_mat = random_double();
            point3 center(a + 0.9*random_double(), 0.2, b + 0.9*random_double());

            if ((center - point3(4, 0.2, 0)).length() > 0.9) {
                shared_ptr<material> sphere_material;

                if (choose_mat < 0.8) {
                    // diffuse
                    auto albedo = color::random_vector() * color::random_vector();
                    sphere_material = make_shared<lambertian>(albedo);
                    auto new_center = center + vec3(0, random_double(0,0.5), 0);
                    world.add(make_shared<sphere>(center, new_center, 0.2, sphere_material));
                } else if (choose_mat < 0.95) {
                    // metal
                    auto albedo = color::random_vector(0.5, 1);
                    auto fuzz = random_double(0, 0.5);
                    sphere_material = make_shared<metal>(albedo, fuzz);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                } else {
                    // glass
                    sphere_material = make_shared<dielectric>(1.5);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                }
            }
        }
    }

    auto material1 = make_shared<dielectric>(1.5);
    world.add(make_shared<sphere>(point3(0, 1, 0), 1.0, material1));

    auto material2 = make_shared<lambertian>(color(0.4, 0.2, 0.1));
    world.add(make_shared<sphere>(point3(-4, 1, 0), 1.0, material2));

    auto material3 = make_shared<metal>(color(0.7, 0.6, 0.5), 0.0);
    world.add(make_shared<sphere>(point3(4, 1, 0), 1.0, material3));

    camera cam;

    cam.aspect_ratio      = 16.0 / 9.0;
    cam.IMAGE_WIDTH       = 1200;
    cam.NUM_SAMPLES_PER_PIXELS = 100;
    cam.MAX_RECURSION_DEPTH         = 50;

    cam.VERTICAL_POV     = 20;
    cam.POV_OF_CAMERA = point3(13,2,3);
    cam.POV_OF_SCENE   = point3(0,0,0);
    cam.UP = vec3(0,1,0);

    cam.DEFOCUS_ANGLE = 0.6;
    cam.FOCUS_DISTANCE    = 10.0;

    cam.render(world);
}

