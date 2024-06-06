//
// Created by Aryan Singh on 1/7/24.
//

#include "Header_Files/color.h"
#include "Header_Files/vec3.h"
#include "Header_Files/constants.h"
#include "Header_Files/sphere.h"
#include "Header_Files/entity.h"
#include "Header_Files/entity_list.h"
#include "Header_Files/ray.h"
#include "Header_Files/camera.h"
#include "Header_Files/material.h"
#include <iostream>

using namespace std;

int main() {
    // World building
    entity_list world;

    auto ground_material = make_shared<lambertian>(color(0.5, 0.5, 0.5));
    world.add(make_shared<sphere>(point3(0,-1000,0), 1000, ground_material));

    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            auto choose_mat = random_double();
            point3 center(a + 0.9*random_double(), 0.2, b + 0.9*random_double());

            if ((center - point3(4, 0.2, 0)).length() > 0.9) {
                shared_ptr<material> sphere_material;

                if (choose_mat < 0.7) {
                    // diffuse
                    auto albedo = color::random_vector() * color::random_vector();
                    sphere_material = make_shared<lambertian>(albedo);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                } else if (choose_mat < 0.85) {
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

    auto material2 = make_shared<lambertian>(color::random_vector());
    world.add(make_shared<sphere>(point3(-4, 1, 0), 1.0, material2));

    auto material3 = make_shared<metal>(color(0.7, 0.6, 0.5), 0.0);
    world.add(make_shared<sphere>(point3(4, 1, 0), 1.0, material3));

    auto material4 = make_shared<metal>(color(0.8, 0.8, 0.8), 0.1);
    world.add(make_shared<sphere>(point3(-2, 1, 2), 1.0, material4));

    auto material5 = make_shared<dielectric>(1.5);
    world.add(make_shared<sphere>(point3(2, 1, -2), 1.0, material5));

    camera cam;
    cam.aspect_ratio = 16.0 / 9.0;
    cam.IMAGE_WIDTH = 1200;
    cam.NUM_SAMPLES_PER_PIXELS = 10;
    cam.MAX_RECURSION_DEPTH = 50;
    cam.VERTICAL_POV = 20;
    cam.POV_OF_SCENE = point3(0,0,0);
    cam.POV_OF_CAMERA = point3(12,2,3);
    cam.UP = vec3(0,1,0);
    cam.DEFOCUS_ANGLE = 0.6;
    cam.FOCUS_DISTANCE = 10.0;
    cam.render(world);
    return 0;
}
