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
#include "bs_thread_pool.h"
#include "sphere.h"
#include <mutex>
#include <thread>
#include <queue>
#include <sstream>
#include <atomic>
#include "thread_pool.h"

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

    struct ThreadInfo {
        int start_col;
        int end_col;
        int start_row;
        int end_row;
    };

    void render(const entity& world) {
        auto start_time = chrono::high_resolution_clock::now();
        initialize();
        cout << "P3\n" << IMAGE_WIDTH << " " << IMAGE_HEIGHT << "\n255\n";

        const int number_of_threads = thread::hardware_concurrency();
        vector<thread> threads;
        mutex queue_mutex;
        mutex output_mutex;
        queue<ThreadInfo> wait_queue;
        vector<vector<array<char, 32>>> buffer(IMAGE_HEIGHT, vector<array<char, 32>>(IMAGE_WIDTH));
        atomic<int> completed(0);
        const int block_size = 12;

        assert(IMAGE_WIDTH % block_size == 0);

        // Assign sub-areas of the image
        for (int row = 0; row < IMAGE_HEIGHT; row += block_size) {
            for (int col = 0; col < IMAGE_WIDTH; col += block_size) {
                ThreadInfo ti = {col, min(col + block_size, IMAGE_WIDTH), row, min(row + block_size, IMAGE_HEIGHT)};
                wait_queue.push(ti);
            }
        }

        // Initialize thread pool
        BS::thread_pool pool(number_of_threads);

        // Lambda function to process a block
        auto process_block = [this, &world, &buffer, &wait_queue, &completed, &queue_mutex, &output_mutex]() {
            while (true) {
                ThreadInfo curr_ti;
                {
                    lock_guard<mutex> lock(queue_mutex);
                    if (wait_queue.empty()) break;
                    curr_ti = wait_queue.front();
                    wait_queue.pop();
                }

                for (int row = curr_ti.start_row; row < curr_ti.end_row; row++) {
                    for (int col = curr_ti.start_col; col < curr_ti.end_col; col++) {
                        color pixel_color(0, 0, 0);
                        for (int s = 0; s < NUM_SAMPLES_PER_PIXELS; s++) {
                            ray r = get_ray(col, row);
                            pixel_color += ray_color(r, world, MAX_RECURSION_DEPTH);
                        }
                        string pixel_str = format_color(pixel_color * sample_scale);
                        copy(pixel_str.begin(), pixel_str.end(), buffer[row][col].begin());
                    }
                }

                {
                    lock_guard<mutex> lock(output_mutex);
                    completed.fetch_add(1);
                    clog << "\rBlocks completed: " << completed.load() << "/" << (IMAGE_WIDTH * IMAGE_HEIGHT) / (block_size * block_size) << flush;
                }
            }
        };

        // Push tasks to the thread pool
        for (int i = 0; i < number_of_threads; ++i) {
            pool.detach_task(process_block);
        }

        // Wait for all tasks to complete
        pool.wait();

        // Output the final image
        for (const auto& row : buffer) {
            for (const auto& cell : row) {
                cout.write(cell.data(), strlen(cell.data()));
            }
        }

        clog << "\r Blocks completed: " << (IMAGE_WIDTH * IMAGE_HEIGHT) / (block_size * block_size) << "/" << (IMAGE_WIDTH * IMAGE_HEIGHT) / (block_size * block_size) << "\n";
        clog << "\r Done.\n" << flush;
        auto end_time = chrono::high_resolution_clock::now();
        auto elapsed_time = chrono::duration_cast<chrono::milliseconds>(end_time - start_time);
        clog << "Rendering time: " << elapsed_time.count() << " milliseconds\n";
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
            ray scattered;
            color change;
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
        double time = random_double();
        return ray(origin, dir, time);
    }

    point3 sample_from_defocus_disk() const {
        auto random_p = random_in_disk();
        return camera_center + (random_p[0] * disk_hr) + (random_p[1] * disk_vr);
    }

    static vec3 random_in_disk() {
        while (true) {
            auto p = vec3(random_double(-1,1), random_double(-1,1), 0);
            if (p.length_squared() >= 1) continue;
            return p;
        }
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
