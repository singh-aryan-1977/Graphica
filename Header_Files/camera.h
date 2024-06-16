//
// Created by Aryan Singh on 5/29/24.
//

#ifndef GRAPHICA_CAMERA_H
#define GRAPHICA_CAMERA_H
#include "entity.h"
#include "constants.h"
#include "entity_list.h"
#include "color.h"
#include "ThreadPool.h"
#include "material.h"
#include "bs_thread_pool.h"
#include "sphere.h"
#include <mutex>
#include <thread>
#include <future>
#include "sstream"
class camera {
public:
    double ASPECT_RATIO = 16.0/9.0;
    int IMAGE_WIDTH = 400;
    int NUM_SAMPLES_PER_PIXELS = 15;
    int MAX_RECURSION_DEPTH = 10;

    double VERTICAL_POV = 90; // vertical view angle
    point3 POV_OF_CAMERA = point3(0,0,0); // point where camera is placed
    point3 POV_OF_SCENE = point3(0,0,-1); // point where scene is located
    vec3 UP = vec3(0,1,0); // up direction relative to camera
    double DEFOCUS_ANGLE = 0;
    double FOCUS_DISTANCE = 10; // distance from camera to perfect focus
    color BACKGROUND;

    struct ThreadInfo {
        int start_col;
        int end_col;
        int start_row;
        int end_row;
    };

//    void render(const entity& world) {
//        auto start_time = std::chrono::high_resolution_clock::now();
//        initialize();
//        std::cout << "P3\n" << IMAGE_WIDTH << " " << IMAGE_HEIGHT << "\n255\n";
//
//        BS::thread_pool pool(std::thread::hardware_concurrency());
//
//        std::vector<std::future<void>> futures;
//        std::vector<std::vector<std::array<char, 32>>> buffer(IMAGE_HEIGHT, std::vector<std::array<char, 32>>(IMAGE_WIDTH));
//        std::atomic<int> completed(0);
//        const int block_size = 8;
//
//        for (int j = 0; j < IMAGE_HEIGHT; j += block_size) {
//            for (int i = 0; i < IMAGE_WIDTH; i += block_size) {
//                futures.push_back(pool.submit_task([this, &world, &buffer, &completed, i, j, block_size]() {
//                    render_block(world, buffer, completed, i, j, block_size);
//                }));
//            }
//        }
//
//        for (auto& future : futures) {
//            future.get();
//        }
//
//        for (const auto& row : buffer) {
//            for (const auto& cell : row) {
//                std::cout.write(&cell[0], std::strlen(&cell[0]));
//            }
//        }
//
//        auto end_time = std::chrono::high_resolution_clock::now();
//        auto elapsed_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
//        std::clog << "Rendering time: " << elapsed_time.count() << " milliseconds\n";
//    }

    void render(const entity& world) {
        auto start_time = std::chrono::high_resolution_clock::now();
        initialize();
        std::cout << "P3\n" << IMAGE_WIDTH << " " << IMAGE_HEIGHT << "\n255\n";

        BS::thread_pool pool(std::thread::hardware_concurrency());

        std::vector<std::vector<std::array<char, 32>>> buffer(IMAGE_HEIGHT,
                                                              std::vector<std::array<char, 32>>(IMAGE_WIDTH));
        std::atomic<int> completed(0);
        const int block_size = 8;

        std::vector<std::future<void>> futures;

        for (int j = 0; j < IMAGE_HEIGHT; j += block_size) {
            for (int i = 0; i < IMAGE_WIDTH; i += block_size) {
                futures.push_back(pool.submit_task([this, &world, &buffer, &completed, i, j, block_size]() {
                    render_block(world, buffer, completed, i, j, block_size);
                }));
            }
        }

        for (auto& future : futures) {
            future.wait();
        }

        for (const auto& row : buffer) {
            for (const auto& cell : row) {
                std::cout.write(&cell[0], std::strlen(&cell[0]));
            }
        }

        auto end_time = std::chrono::high_resolution_clock::now();
        auto elapsed_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
        std::clog << "Rendering time: " << elapsed_time.count() << " milliseconds\n";
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
    int sqrt_samples_per_pixel;
    double recip_sqrt_samples_per_pixel;

    void initialize() {
        IMAGE_HEIGHT = static_cast<int>(IMAGE_WIDTH / ASPECT_RATIO);
        IMAGE_HEIGHT = (IMAGE_HEIGHT < 1) ? 1 : IMAGE_HEIGHT;

        sqrt_samples_per_pixel = int(sqrt(NUM_SAMPLES_PER_PIXELS));
        // setting sample scale for anti-aliasing
        sample_scale = 1.0 / (sqrt_samples_per_pixel * sqrt_samples_per_pixel);
        recip_sqrt_samples_per_pixel = 1.0 / sqrt_samples_per_pixel;

        // World building
        entity_list world;

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


    void render_block(const entity& world, std::vector<std::vector<std::array<char, 32>>>& buffer, std::atomic<int>& completed, int start_col, int start_row, int block_size) {
        for (int j = start_row; j < start_row + block_size && j < IMAGE_HEIGHT; ++j) {
            for (int i = start_col; i < start_col + block_size && i < IMAGE_WIDTH; ++i) {
                color pixel_color(0, 0, 0);
                for (int s_i = 0; s_i < sqrt_samples_per_pixel; s_i++) {
                    for (int s_j = 0; s_j < sqrt_samples_per_pixel; s_j++) {
                        ray r = get_ray(i, j, s_i, s_j);
                        pixel_color += ray_color(r, world, MAX_RECURSION_DEPTH);
                    }
                }
                std::ostringstream oss;
                write_color(oss, pixel_color * sample_scale);
                std::string pixel_str = oss.str();
                std::copy(pixel_str.begin(), pixel_str.end(), buffer[j][i].begin());
            }
        }
        completed.fetch_add(1);
        std::clog << "\rBlocks completed: " << completed.load() << "/" << (IMAGE_WIDTH * IMAGE_HEIGHT) / (block_size * block_size) << std::flush;
    }


    color ray_color(const ray& r, const entity& world, int curr_depth) const {
//        clog << "Entered ray color \n";
        if (curr_depth <= 0) {
//            clog << "Exit ray color through max depth \n";
            return color(0,0,0);
        }
        entity_record record;
        if (world.hit(r, interval(0.001, inf), record)) {
            ray scattered;
            color change;
            color emitted_color = record.materials->emit(record.u, record.v, record.p);
            if (!record.materials->scatter(r, record, change, scattered)) {
//                clog << "Exited ray color with no scatter \n";
                return emitted_color;
            }
//            clog << "Exited ray color through recursio\n";
            color scattered_color = change * ray_color(scattered, world, curr_depth-1);
            return scattered_color + emitted_color;
        }
//        vec3 unit_dir = unit_vector(r.direction());
//        auto a = 0.5*(unit_dir.y()+1.0);
//        clog << "Exited ray color through no hit \n";
//        return (1.0-a)*color(1.0, 1.0, 1.0) + a*color(0.5,0.7,1.0);
        return BACKGROUND;
    }



    static vec3 sample_for_antialiasing() {
        return {random_double() - 0.5, random_double() - 0.5, 0};
    }

    // Function for generating origin camera ray from defocus disk and pointing towards random points in square around pixel
    // used for anti-aliasing
    ray get_ray(int col, int row, int s_i, int s_j) const {
        auto offset_square = sample_from_stratified(s_i, s_j);
//        auto offset_square = sample_for_antialiasing();
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

    vec3 sample_from_stratified(int s_i, int s_j) const {

        auto px = ((s_i + random_double()) * recip_sqrt_samples_per_pixel) - 0.5;
        auto py = ((s_j + random_double()) * recip_sqrt_samples_per_pixel) - 0.5;

        return vec3(px, py, 0);
    }

    static string format_color(const color& color) {
        ostringstream ss;
        ss << static_cast<int>(255.999 * color.x()) << ' '
           << static_cast<int>(255.999 * color.y()) << ' '
           << static_cast<int>(255.999 * color.z()) << '\n';
        return ss.str();
    }
};
#endif //GRAPHICA_CAMERA_H