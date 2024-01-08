//
// Created by Aryan Singh on 1/8/24.
//

#ifndef GRAPHICA_COLOR_H
#define GRAPHICA_COLOR_H

#include "Header_Files/vec3.h"

#include <iostream>
using namespace std;

using color = vec3;

void write_color(std::ostream &out, color pixel_color) {
    out << static_cast<int>(255.999 * pixel_color.x()) << ' '
        << static_cast<int>(255.999 * pixel_color.y()) << ' '
        << static_cast<int>(255.999 * pixel_color.z()) << '\n';
}

#endif //GRAPHICA_COLOR_H
