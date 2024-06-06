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
#include <mutex>
#include <thread>

class camera {
public:
    double aspect_ratio = 16.0/9.0;
    int IMAGE_WIDTH = 400;
    int NUM_SAMPLES_PER_PIXELS = 15;
    int MAX_RECURSION_DEPTH = 10;

    double VERTICAL_POV = 90; // vertical view angle
    point3 POV_OF_CAMERA = point3(0,0,0); // point where camera is placed
    point3 POV_OF_SCENE = point3(0,0,-1); // point where scene is located
    vec3 UP = vec3(0,1,0); // up direction relative to camera
    double DEFOCUS_ANGLE = 0;
    double FOCUS_DISTANCE = 10; // distance from camera to perfect focus

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
    vec3 u, v, w; // basis vectors for camera plane
    vec3 disk_hr; // horizontal radius for defocus disk
    vec3 disk_vr; // vertical radius for defocus disk

    void initialize() {
        IMAGE_HEIGHT = static_cast<int>(IMAGE_WIDTH / aspect_ratio);
        IMAGE_HEIGHT = (IMAGE_HEIGHT < 1) ? 1 : IMAGE_HEIGHT;

        // World building
        entity_list world;
//        world.add(make_shared<sphere>(point3(0,0,-1),0.5));
//        world.add(make_shared<sphere>(point3(0,-100.5,-1),100));


        // setting sample scale for anti-aliasing
        sample_scale = 1.0 / NUM_SAMPLES_PER_PIXELS;

        // setting camera and viewport dimensions
        auto h = tan(deg_to_rad(VERTICAL_POV) / 2);
        auto view_height = 2.0 * h * FOCUS_DISTANCE;
        auto view_width = view_height * (static_cast<double>(IMAGE_WIDTH) / IMAGE_HEIGHT);
        camera_center = POV_OF_CAMERA;

        w = unit_vector(POV_OF_CAMERA - POV_OF_SCENE);
        u = unit_vector(cross(UP, w));
        v = cross(w, u);

        // vectors for horizontal and vertical viewport edges
        auto viewport_vector_u = view_width * u;
        auto viewport_vector_v = view_height * -v;

        // calculating delta vectors
        pixel_delta_u = viewport_vector_u / IMAGE_WIDTH;
        pixel_delta_v = viewport_vector_v / IMAGE_HEIGHT;

        // calculate location of pixel at (0,0)
        auto view_upper_left = camera_center - (FOCUS_DISTANCE * w) - viewport_vector_u / 2 - viewport_vector_v / 2;
        pixel_0_loc = view_upper_left + (0.5 * (pixel_delta_u + pixel_delta_v));

        // defocus disk
        auto defocus_r = FOCUS_DISTANCE * tan(deg_to_rad(DEFOCUS_ANGLE / 2));
        disk_hr = defocus_r * u;
        disk_vr = defocus_r * v;
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

    // Function for generating origin camera ray from defocus disk and pointing towards random points in square around pixel
    // used for anti-aliasing
    ray get_ray(int col, int row) const {
        auto offset_square = sample_for_antialiasing();
        auto pixel_location = pixel_0_loc + ((col + offset_square.x()) * pixel_delta_u) + ((row + offset_square.y()) * pixel_delta_v);

        point3 origin;
        if (DEFOCUS_ANGLE <= 0) {
            origin = camera_center;
        } else {
            origin = sample_from_defocus_disk();
        }
        auto dir = pixel_location-origin;
        return ray(origin, dir);
    }

    point3 sample_from_defocus_disk() const {
        auto random_p = random_in_disk();
        return camera_center + (random_p[0] * disk_hr) + (random_p[1] * disk_vr);
    }
};
#endif //GRAPHICA_CAMERA_H
