//
// Created by Aryan Singh on 5/29/24.
//

#ifndef GRAPHICA_CAMERA_H
#define GRAPHICA_CAMERA_H
#include "entity.h"
#include "constants.h"
#include "entity_list.h"
#include "color.h"
#include "material.h"
#include "sphere.h"

class camera {
public:
    double aspect_ratio = 16.0/9.0;
    int IMAGE_WIDTH = 400;
    int NUM_SAMPLES_PER_PIXELS = 15;
    int MAX_RECURSION_DEPTH = 10;
    void render(const entity& world) {
        initialize();
        cout << "P3\n" << IMAGE_WIDTH << " " << IMAGE_HEIGHT << "\n255\n";
        for (int i = 0; i < IMAGE_HEIGHT; i++){
            clog << "\rScan lines remaining: " << (IMAGE_HEIGHT - i) << ' ' << std::flush;
            for (int j = 0; j < IMAGE_WIDTH; j++) {
//                auto pixel_center = pixel_0_loc + (j * pixel_delta_u) + (i * pixel_delta_v);
//                auto ray_direction = pixel_center - camera_center;
//                ray r(camera_center, ray_direction);
//                color pixel_color = ray_color(r, world);

                color pixel_color(0, 0, 0); // initial value
                for (int s = 0; s < NUM_SAMPLES_PER_PIXELS; s++) {
                    ray r  = get_ray(j, i);
                    pixel_color += ray_color(r, world, MAX_RECURSION_DEPTH);
                }
//                clog << sample_scale << "\n";
                write_color(std::cout, pixel_color * sample_scale);
            }
        }
        clog << "\rDone.\n";

    }
private:
    int IMAGE_HEIGHT;
    point3 camera_center;
    point3 pixel_0_loc;
    vec3 pixel_delta_u;
    double sample_scale;
    vec3 pixel_delta_v; // just checking

    void initialize() {
        IMAGE_HEIGHT = static_cast<int>(IMAGE_WIDTH/aspect_ratio);
        IMAGE_HEIGHT = (IMAGE_HEIGHT < 1) ? 1 : IMAGE_HEIGHT;

        // World building
        entity_list world;
//        world.add(make_shared<sphere>(point3(0,0,-1),0.5));
//        world.add(make_shared<sphere>(point3(0,-100.5,-1),100));


        // setting sample scale for anti-aliasing
        sample_scale = 1.0 / NUM_SAMPLES_PER_PIXELS;

        // setting camera and viewport dimensions
        auto focal_length = 1.0;
        auto view_height = 2.0;
        auto view_width = view_height * (static_cast<double>(IMAGE_WIDTH)/IMAGE_HEIGHT);
        camera_center = point3(0,0,0);

        // vectors for horizontal and vertical viewport edges
        auto viewport_vector_u = vec3(view_width, 0, 0);
        auto viewport_vector_v = vec3(0,-view_height,0);

        // calculating delta vectors
        pixel_delta_u = viewport_vector_u / IMAGE_WIDTH;
        pixel_delta_v = viewport_vector_v / IMAGE_HEIGHT;

        // calculate location of pixel at (0,0)
        auto view_upper_left = camera_center-vec3(0,0,focal_length)-viewport_vector_u/2 - viewport_vector_v/2;
        pixel_0_loc = view_upper_left + (0.5 * (pixel_delta_u + pixel_delta_v));
    }

    color ray_color(const ray& r, const entity& world, int curr_depth) const {
        if (curr_depth <= 0) {
            return color(0,0,0);
        }
        entity_record record;
        if (world.hit(r, interval(0.00001, inf), record)) {
//            clog << "Hit something in the world" << "\n";
//            vec3 direction = check_orientation(record.normal);
//            return 0.5 * (record.normal + color(1,1,1));
            ray scattered;
            color change;
//            vec3 direction = record.normal + normalize_vec_in_unit_sphere(); // Lambertian reflection
//            return 0.5 * ray_color(ray(record.p, direction), world, curr_depth-1);
            if (record.materials->scatter(r, record, change, scattered)) {
                return change * ray_color(scattered, world, curr_depth - 1);
            }
            return color(0,0,0);
        }
        vec3 unit_dir = unit_vector(r.direction());
        auto a = 0.5*(unit_dir.y()+1.0);
        return (1.0-a)*color(1.0, 1.0, 1.0) + a*color(0.5,0.7,1.0);
    }

    static vec3 sample_for_antialiasing() {
        return {random_double() - 0.5, random_double() - 0.5, 0};
    }

    // Function for generating origin camera ray and pointing towards random points in square around pixel
    // used for anti-aliasing
    ray get_ray(int col, int row) const {
        auto offset_square = sample_for_antialiasing();
        auto pixel_location = pixel_0_loc + ((col + offset_square.x()) * pixel_delta_u) + ((row + offset_square.y()) * pixel_delta_v);

        auto origin = camera_center;
        auto dir = pixel_location-origin;
        return ray(origin, dir);
    }
};
#endif //GRAPHICA_CAMERA_H
