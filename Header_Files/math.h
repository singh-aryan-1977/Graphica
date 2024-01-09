//
// Created by Aryan Singh on 1/9/24.
//

#ifndef GRAPHICA_MATH_H
#define GRAPHICA_MATH_H
#include "ray.h"
#include "vec3.h"
#include <cmath>
#include <limits>
#include <memory>


using namespace std;

// Constants
const double inf = numeric_limits<double>::infinity();
const double pi = 3.141592652589793238462643383279;

// Utility functions
inline double deg_to_rad(double deg) {
    return (deg * pi) / 180.0;
}



#endif //GRAPHICA_MATH_H
