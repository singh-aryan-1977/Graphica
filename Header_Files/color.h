//
// Created by Aryan Singh on 1/8/24.
//

#ifndef GRAPHICA_COLOR_H
#define GRAPHICA_COLOR_H

//#include "Header_Files/vec3.h"
#include "interval.h"
#include "vec3.h"

#include <iostream>
using namespace std;

using color = vec3;

inline double linear_space_to_gamma_space(double linear) {
    if (linear > 0.0) {
        return sqrt(linear); // can we use other gamma e.g. gamma-3
    }
    return 0.0;
}

void write_color(std::ostream &out, color pixel_color) {
    auto red_component = pixel_color.x();
    auto green_component = pixel_color.y();
    auto blue_component = pixel_color.z();

    // for NaN
    if (red_component != red_component) {
        red_component = 0.0;
    }
    if (green_component != green_component) {
        green_component = 0.0;
    }
    if (blue_component != blue_component) {
        blue_component = 0.0;
    }
    // convert to gamma
    red_component = linear_space_to_gamma_space(red_component);
    green_component = linear_space_to_gamma_space(green_component);
    blue_component = linear_space_to_gamma_space(blue_component);

    static const interval color_intensity(0.000, 0.999);
    int red = int(256 * color_intensity.bound_to(red_component)); // to ensure that values are between 0.0->0.9
    int green = int(256 * color_intensity.bound_to(green_component));
    int blue = int(256 * color_intensity.bound_to(blue_component));
//    clog << red << ' ' << green << ' ' << blue << "\n";
    out << red << ' ' << green << ' ' << blue << '\n';
}

#endif //GRAPHICA_COLOR_H
