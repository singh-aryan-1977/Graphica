#include "Header_Files/color.h"
#include "Header_Files/vec3.h"
#include "Header_Files/constants.h"
#include "Header_Files/sphere.h"
#include "Header_Files/entity.h"
#include "Header_Files/entity_list.h"
#include "Header_Files/ray.h"
#include "Header_Files/texture.h"
#include "Header_Files/camera.h"
#include "Header_Files/material.h"
#include "Header_Files/pyramid.h"
#include "Header_Files/volumes.h"
#include "Header_Files/bvh.h"
#include "Header_Files/quadrilateral.h"
#include <iostream>

using namespace std;

void bouncing_spheres() {
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


    world = entity_list(make_shared<bvh>(world));
    camera cam;

    cam.ASPECT_RATIO      = 16.0 / 9.0;
    cam.IMAGE_WIDTH       = 1200;
    cam.NUM_SAMPLES_PER_PIXELS = 100;
    cam.MAX_RECURSION_DEPTH         = 50;

    cam.VERTICAL_POV     = 20;
    cam.POV_OF_CAMERA = point3(13,2,3);
    cam.POV_OF_SCENE   = point3(0,0,0);
    cam.UP = vec3(0,1,0);

    cam.DEFOCUS_ANGLE = 0.6;
    cam.FOCUS_DISTANCE    = 10.0;
    cam.BACKGROUND = color(0.70, 0.80, 1.00);

    cam.render(world);
}

void checkered_spheres() {
    entity_list world;

    auto checkered = make_shared<checker>(0.32, color(.2, .3, .1), color(.9, .9, .9));

    world.add(make_shared<sphere>(point3(0,-10, 0), 10, make_shared<lambertian>(checkered)));
    world.add(make_shared<sphere>(point3(0, 10, 0), 10, make_shared<lambertian>(checkered)));

    camera cam;

    cam.ASPECT_RATIO      = 16.0 / 9.0;
    cam.IMAGE_WIDTH       = 400;
    cam.NUM_SAMPLES_PER_PIXELS = 100;
    cam.MAX_RECURSION_DEPTH         = 50;

    cam.VERTICAL_POV     = 20;
    cam.POV_OF_CAMERA = point3(13,2,3);
    cam.POV_OF_SCENE   = point3(0,0,0);
    cam.UP = vec3(0,1,0);

    cam.DEFOCUS_ANGLE = 0;
    cam.BACKGROUND = color(0.70, 0.80, 1.00);

    cam.render(world);
}

void earth() {
    auto earth_texture = make_shared<image_texture>("./image_textures/earthmap.jpg");
    auto surface = make_shared<lambertian>(earth_texture);
    auto globe = make_shared<sphere>(point3(0,0,0), 2, surface);

    camera cam;

    cam.ASPECT_RATIO      = 16.0 / 9.0;
    cam.IMAGE_WIDTH       = 400;
    cam.NUM_SAMPLES_PER_PIXELS = 100;
    cam.MAX_RECURSION_DEPTH         = 50;

    cam.VERTICAL_POV     = 20;
    cam.POV_OF_CAMERA = point3(0,0,12);
    cam.POV_OF_SCENE   = point3(0,0,0);
    cam.UP = vec3(0,1,0);

    cam.DEFOCUS_ANGLE = 0;
    cam.BACKGROUND = color(0.70, 0.80, 1.00);

    cam.render(entity_list(globe));
}

void perlin_spheres() {
    entity_list world;
    auto texture = make_shared<noise_texture>(4);
    world.add(make_shared<sphere>(point3(0,-1000,0), 1000, make_shared<lambertian>(texture)));
    world.add(make_shared<sphere>(point3(0,2,0), 2, make_shared<lambertian>(texture)));

    camera cam;

    cam.ASPECT_RATIO      = 16.0 / 9.0;
    cam.IMAGE_WIDTH       = 400;
    cam.NUM_SAMPLES_PER_PIXELS = 100;
    cam.MAX_RECURSION_DEPTH         = 50;

    cam.VERTICAL_POV     = 20;
    cam.POV_OF_CAMERA = point3(12,2,3);
    cam.POV_OF_SCENE   = point3(0,0,0);
    cam.UP = vec3(0,1,0);

    cam.DEFOCUS_ANGLE = 0;
    cam.BACKGROUND = color(0.70, 0.80, 1.00);

    cam.render(world);
}

void quads() {
    entity_list world;

    // Materials
    auto left_red     = make_shared<lambertian>(color(1.0, 0.2, 0.2));
    auto back_green   = make_shared<lambertian>(color(0.2, 1.0, 0.2));
    auto right_blue   = make_shared<lambertian>(color(0.2, 0.2, 1.0));
    auto upper_orange = make_shared<lambertian>(color(1.0, 0.5, 0.0));
    auto lower_teal   = make_shared<lambertian>(color(0.2, 0.8, 0.8));

    // Quads
    world.add(make_shared<quadrilateral>(point3(-3,-2, 5), vec3(0, 0,-4), vec3(0, 4, 0), left_red));
    world.add(make_shared<quadrilateral>(point3(-2,-2, 0), vec3(4, 0, 0), vec3(0, 4, 0), back_green));
    world.add(make_shared<quadrilateral>(point3( 3,-2, 1), vec3(0, 0, 4), vec3(0, 4, 0), right_blue));
    world.add(make_shared<quadrilateral>(point3(-2, 3, 1), vec3(4, 0, 0), vec3(0, 0, 4), upper_orange));
    world.add(make_shared<quadrilateral>(point3(-2,-3, 5), vec3(4, 0, 0), vec3(0, 0,-4), lower_teal));

    camera cam;

    cam.ASPECT_RATIO      = 1.0;
    cam.IMAGE_WIDTH       = 400;
    cam.NUM_SAMPLES_PER_PIXELS = 100;
    cam.MAX_RECURSION_DEPTH         = 50;

    cam.VERTICAL_POV     = 80;
    cam.POV_OF_CAMERA = point3(0,0,9);
    cam.POV_OF_SCENE   = point3(0,0,0);
    cam.UP = vec3(0,1,0);

    cam.DEFOCUS_ANGLE = 0;
    cam.BACKGROUND = color(0.70, 0.80, 1.00);
    cam.render(world);
}

void simple_light() {
    entity_list world;

    auto perlin = make_shared<noise_texture>(4);
    world.add(make_shared<sphere>(point3(0,-1000,0), 1000, make_shared<lambertian>(perlin)));
    world.add(make_shared<sphere>(point3(0,2,0), 2, make_shared<lambertian>(perlin)));

    auto light_source = make_shared<diffuse_light>(color(4,4,4));
    world.add(make_shared<sphere>(point3(0,7,0), 2, light_source));
    world.add(make_shared<quadrilateral>(point3(3,1,-2), vec3(2,0,0), vec3(0,2,0), light_source));

    camera cam;

    cam.ASPECT_RATIO      = 16.0 / 9.0;
    cam.IMAGE_WIDTH       = 400;
    cam.NUM_SAMPLES_PER_PIXELS = 100;
    cam.MAX_RECURSION_DEPTH         = 50;

    cam.VERTICAL_POV     = 20;
    cam.POV_OF_CAMERA = point3(26,3,6);
    cam.POV_OF_SCENE   = point3(0,2,0);
    cam.UP = vec3(0,1,0);

    cam.DEFOCUS_ANGLE = 0;
    cam.BACKGROUND = color(0,0,0);

    cam.render(world);
}

void cornell_box() {
    entity_list world;

    auto red   = make_shared<lambertian>(color(.65, .05, .05));
    auto white = make_shared<lambertian>(color(.73, .73, .73));
    auto green = make_shared<lambertian>(color(.12, .45, .15));
    auto light_source = make_shared<diffuse_light>(color(15, 15, 15));

    world.add(make_shared<quadrilateral>(point3(555,0,0), vec3(0,555,0), vec3(0,0,555), green));
    world.add(make_shared<quadrilateral>(point3(0,0,0), vec3(0,555,0), vec3(0,0,555), red));
    world.add(make_shared<quadrilateral>(point3(343, 554, 332), vec3(-130,0,0), vec3(0,0,-105), light_source));
    world.add(make_shared<quadrilateral>(point3(0,0,0), vec3(555,0,0), vec3(0,0,555), white));
    world.add(make_shared<quadrilateral>(point3(555,555,555), vec3(-555,0,0), vec3(0,0,-555), white));
    world.add(make_shared<quadrilateral>(point3(0,0,555), vec3(555,0,0), vec3(0,555,0), white));

    shared_ptr<entity> box1 = box(point3(0,0,0), point3(165,330,165), white);
    box1 = make_shared<rotate_y>(box1, 15);
    box1 = make_shared<translate>(box1, vec3(265,0,295));
    world.add(box1);

    shared_ptr<entity> box2 = box(point3(0,0,0), point3(165,165,165), white);
    box2 = make_shared<rotate_y>(box2, -18);
    box2 = make_shared<translate>(box2, vec3(130,0,65));
    world.add(box2);
    camera cam;

    cam.ASPECT_RATIO      = 1.0;
    cam.IMAGE_WIDTH       = 600;
    cam.NUM_SAMPLES_PER_PIXELS = 200;
    cam.MAX_RECURSION_DEPTH         = 50;

    cam.VERTICAL_POV     = 40;
    cam.POV_OF_CAMERA = point3(278,278,-800);
    cam.POV_OF_SCENE   = point3(278,278,0);
    cam.UP = vec3(0,1,0);

    cam.DEFOCUS_ANGLE = 0;
    cam.BACKGROUND = color(0,0,0);

    cam.render(world);;
}

void cornell_smoke() {
    entity_list world;

    auto red   = make_shared<lambertian>(color(.65, .05, .05));
    auto white = make_shared<lambertian>(color(.73, .73, .73));
    auto green = make_shared<lambertian>(color(.12, .45, .15));
    auto light_source = make_shared<diffuse_light>(color(7, 7, 7));

    world.add(make_shared<quadrilateral>(point3(555,0,0), vec3(0,555,0), vec3(0,0,555), green));
    world.add(make_shared<quadrilateral>(point3(0,0,0), vec3(0,555,0), vec3(0,0,555), red));
    world.add(make_shared<quadrilateral>(point3(113,554,127), vec3(330,0,0), vec3(0,0,305), light_source));
    world.add(make_shared<quadrilateral>(point3(0,555,0), vec3(555,0,0), vec3(0,0,555), white));
    world.add(make_shared<quadrilateral>(point3(0,0,0), vec3(555,0,0), vec3(0,0,555), white));
    world.add(make_shared<quadrilateral>(point3(0,0,555), vec3(555,0,0), vec3(0,555,0), white));

    shared_ptr<entity> box1 = box(point3(0,0,0), point3(165,330,165), white);
    box1 = make_shared<rotate_y>(box1, 15);
    box1 = make_shared<translate>(box1, vec3(265,0,295));

    shared_ptr<entity> box2 = box(point3(0,0,0), point3(165,165,165), white);
    box2 = make_shared<rotate_y>(box2, -18);
    box2 = make_shared<translate>(box2, vec3(130,0,65));

    world.add(make_shared<volumes>(box1, 0.01, color(0,0,0)));
    world.add(make_shared<volumes>(box2, 0.01, color(1,1,1)));
    camera cam;

    cam.ASPECT_RATIO      = 1.0;
    cam.IMAGE_WIDTH       = 600;
    cam.NUM_SAMPLES_PER_PIXELS = 200;
    cam.MAX_RECURSION_DEPTH         = 50;

    cam.VERTICAL_POV     = 40;
    cam.POV_OF_CAMERA = point3(278,278,-800);
    cam.POV_OF_SCENE   = point3(278,278,0);
    cam.UP = vec3(0,1,0);


    cam.DEFOCUS_ANGLE = 0;
    cam.BACKGROUND = color(0,0,0);

    cam.render(world);;
}

void final_scene(int image_width, int samples_per_pixel, int max_recursion) {

    entity_list boxes1;
    auto ground = make_shared<lambertian>(color(0.48, 0.83, 0.53));

    int boxes_per_side = 20;
    for (int i = 0; i < boxes_per_side; i++) {
        for (int j = 0; j < boxes_per_side; j++) {
            auto w = 100.0;
            auto x0 = -1000.0 + i*w;
            auto z0 = -1000.0 + j*w;
            auto y0 = 0.0;
            auto x1 = x0 + w;
            auto y1 = random_double(1,101);
            auto z1 = z0 + w;

            boxes1.add(box(point3(x0,y0,z0), point3(x1,y1,z1), ground));
        }
    }

    entity_list world;

    world.add(make_shared<bvh>(boxes1));

    auto light_source = make_shared<diffuse_light>(color(7, 7, 7));
    world.add(make_shared<quadrilateral>(point3(123,554,147), vec3(300,0,0), vec3(0,0,265), light_source));

    auto center1 = point3(400, 400, 200);
    auto center2 = center1 + vec3(30,0,0);
    auto sphere_material = make_shared<lambertian>(color(0.7, 0.3, 0.1));
    world.add(make_shared<sphere>(center1, center2, 50, sphere_material));

    world.add(make_shared<sphere>(point3(260, 150, 45), 50, make_shared<dielectric>(1.5)));
    world.add(make_shared<sphere>(
            point3(0, 150, 145), 50, make_shared<metal>(color(0.8, 0.8, 0.9), 1.0)
    ));

    auto boundary = make_shared<sphere>(point3(360,150,145), 70, make_shared<dielectric>(1.5));
    world.add(boundary);
    world.add(make_shared<volumes>(boundary, 0.2, color(0.2, 0.4, 0.9)));
    boundary = make_shared<sphere>(point3(0,0,0), 5000, make_shared<dielectric>(1.5));
    world.add(make_shared<volumes>(boundary, .0001, color(1,1,1)));

    auto emat = make_shared<lambertian>(make_shared<image_texture>("./image_textures/earthmap.jpg"));
    world.add(make_shared<sphere>(point3(400,200,400), 100, emat));
    auto pertext = make_shared<noise_texture>(0.2);
    world.add(make_shared<sphere>(point3(220,280,300), 80, make_shared<lambertian>(pertext)));

    entity_list boxes2;
    auto white = make_shared<lambertian>(color(.73, .73, .73));
    int ns = 1000;
    for (int j = 0; j < ns; j++) {
        boxes2.add(make_shared<sphere>(point3::random_vector(0,165), 10, white));
    }

    world.add(make_shared<translate>(
                      make_shared<rotate_y>(
                              make_shared<bvh>(boxes2), 15),
                      vec3(-100,270,395)
              )
    );

    camera cam;

    cam.ASPECT_RATIO      = 1.0;
    cam.IMAGE_WIDTH       = image_width;
    cam.NUM_SAMPLES_PER_PIXELS = samples_per_pixel;
    cam.MAX_RECURSION_DEPTH         = max_recursion;

    cam.VERTICAL_POV     = 40;
    cam.POV_OF_CAMERA = point3(478,278,-600);
    cam.POV_OF_SCENE   = point3(278,278,0);
    cam.UP = vec3(0,1,0);


    cam.DEFOCUS_ANGLE = 0;
    cam.BACKGROUND = color(0,0,0);

    cam.render(world);;
}

void cornell_stratified() {
    entity_list world;

    auto red   = make_shared<lambertian>(color(.65, .05, .05));
    auto white = make_shared<lambertian>(color(.73, .73, .73));
    auto green = make_shared<lambertian>(color(.12, .45, .15));
    auto light_source = make_shared<diffuse_light>(color(15, 15, 15));

    // Cornell box sides
    world.add(make_shared<quadrilateral>(point3(555,0,0), vec3(0,0,555), vec3(0,555,0), green));
    world.add(make_shared<quadrilateral>(point3(0,0,555), vec3(0,0,-555), vec3(0,555,0), red));
    world.add(make_shared<quadrilateral>(point3(0,555,0), vec3(555,0,0), vec3(0,0,555), white));
    world.add(make_shared<quadrilateral>(point3(0,0,555), vec3(555,0,0), vec3(0,0,-555), white));
    world.add(make_shared<quadrilateral>(point3(555,0,555), vec3(-555,0,0), vec3(0,555,0), white));

    // Light
    world.add(make_shared<quadrilateral>(point3(213,554,227), vec3(130,0,0), vec3(0,0,105), light_source));

    // Box 1
    shared_ptr<entity> box1 = box(point3(0,0,0), point3(165,330,165), white);
    box1 = make_shared<rotate_y>(box1, 15);
    box1 = make_shared<translate>(box1, vec3(265,0,295));
    world.add(box1);

    // Box 2
    shared_ptr<entity> box2 = box(point3(0,0,0), point3(165,165,165), white);
    box2 = make_shared<rotate_y>(box2, -18);
    box2 = make_shared<translate>(box2, vec3(130,0,65));
    world.add(box2);

    camera cam;

    cam.ASPECT_RATIO      = 1.0;
    cam.IMAGE_WIDTH       = 600;
    cam.NUM_SAMPLES_PER_PIXELS = 64;
    cam.MAX_RECURSION_DEPTH         = 50;

    cam.VERTICAL_POV     = 40;
    cam.POV_OF_CAMERA = point3(278,278,-600);
    cam.POV_OF_SCENE   = point3(278,278,0);
    cam.UP = vec3(0,1,0);


    cam.DEFOCUS_ANGLE = 0;
    cam.BACKGROUND = color(0,0,0);

    cam.render(world);;
}
int main() {

    switch(7) {
        case 1: bouncing_spheres(); break;
        case 2: checkered_spheres(); break;
        case 3: earth(); break;
        case 4: perlin_spheres(); break;
        case 5: quads(); break;
        case 6: simple_light(); break;
        case 7: cornell_box(); break;
        case 8: cornell_smoke(); break;
        case 9: final_scene(800, 10000, 40); break;
        default:
            final_scene(800, 500, 4); break;
    }

}

