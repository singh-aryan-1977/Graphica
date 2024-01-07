//
// Created by Aryan Singh on 1/7/24.
//

#include "main.h"
#include <iostream>

using namespace std;

int main() {
    // Dimensions
    int IMAGE_HEIGHT = 256;
    int IMAGE_WIDTH = 256;

    // Render
    cout << "P3\n" << IMAGE_WIDTH << " " << IMAGE_HEIGHT << "\n255\n";
    for (int i = 0; i < IMAGE_HEIGHT; i++){
        std::clog << "\rScanlines remaining: " << (IMAGE_HEIGHT - i) << ' ' << std::flush;
        for (int j = 0; j < IMAGE_WIDTH; j++) {
            auto r = double(j) / (IMAGE_WIDTH-1);
            auto g = double(i) / (IMAGE_HEIGHT-1);
            auto b = 0;

            int ir = static_cast<int>(255.999 * r);
            int ig = static_cast<int>(255.999 * g);
            int ib = static_cast<int>(255.999 * b);

            cout << ir << ' ' << ig << ' ' << ib << "\n";
        }
    }
    std::clog << "\rDone.                 \n";
    return 0;
}
