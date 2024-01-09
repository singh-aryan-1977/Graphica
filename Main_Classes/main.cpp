//
// Created by Aryan Singh on 1/7/24.
//

#include "Header_Files/color.h"
#include "Header_Files/vec3.h"
#include "Header_Files/math.h"
#include "Header_Files/sphere.h"
#include "Header_Files/entity.h"
#include "Header_Files/entity_list.h"
#include "Header_Files/ray.h"
#include <iostream>

using namespace std;

color ray_color(const ray& r, const entity& world) {
    entity_record record;
    if (world.hit(r, 0, inf, record)) {
        return 0.5 * (record.normal + color(1,1,1));
    }
    vec3 unit_dir = unit_vector(r.direction());
    auto a = 0.5*(unit_dir.y()+1.0);
    return (1.0-a)*color(1.0, 1.0, 1.0) + a*color(0.5,0.7,1.0);
}

int main() {
    // Image Dimensions
    auto aspect_ratio = 16.0/9.0;
    int IMAGE_WIDTH = 400;
    int IMAGE_HEIGHT = static_cast<int>(IMAGE_WIDTH/aspect_ratio);
    IMAGE_HEIGHT = (IMAGE_HEIGHT < 1) ? 1 : IMAGE_HEIGHT;

    // World building
    entity_list world;
    world.add(make_shared<sphere>(point3(0,0,-1),0.5));
    world.add(make_shared<sphere>(point3(0,-100.5,-1),100));


    // setting camera and viewport dimensions
    auto focal_length = 1.0;
    auto view_height = 2.0;
    auto view_width = view_height * (static_cast<double>(IMAGE_WIDTH)/IMAGE_HEIGHT);
    auto camera_center = point3(0,0,0);

    // vectors for horizontal and vertical viewport edges
    auto viewport_vector_u = vec3(view_width, 0, 0);
    auto viewport_vector_v = vec3(0,-view_height,0);

    // calculating delta vectors
    auto pixel_delta_u = viewport_vector_u / IMAGE_WIDTH;
    auto pixel_delta_v = viewport_vector_v / IMAGE_HEIGHT;

    // calculate location of pixel at (0,0)
    auto view_upper_left = camera_center-vec3(0,0,focal_length)-viewport_vector_u/2 - viewport_vector_v/2;
    auto pixel_0_loc = view_upper_left + (0.5 * (pixel_delta_u + pixel_delta_v));


    // Render
    cout << "P3\n" << IMAGE_WIDTH << " " << IMAGE_HEIGHT << "\n255\n";
    for (int i = 0; i < IMAGE_HEIGHT; i++){
        std::clog << "\rScan lines remaining: " << (IMAGE_HEIGHT - i) << ' ' << std::flush;
        for (int j = 0; j < IMAGE_WIDTH; j++) {
            auto pixel_center = pixel_0_loc + (j * pixel_delta_u) + (i * pixel_delta_v);
            auto ray_direction = pixel_center - camera_center;
            ray r(camera_center, ray_direction);
            color pixel_color = ray_color(r, world);
            write_color(std::cout, pixel_color);
        }
    }
    std::clog << "\rDone.                 \n";
    return 0;
}
