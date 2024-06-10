//
// Created by Aryan Singh on 1/9/24.
//

#ifndef GRAPHICA_CONSTANTS_H
#define GRAPHICA_CONSTANTS_H

//#include "interval.h"
#include <cmath>
#include <limits>
#include <memory>
#include <random>
#include <time.h>


using namespace std;

// Constants
const double inf = numeric_limits<double>::infinity();
const double pi = 3.141592652589793238462643383279;

// Utility functions
inline double deg_to_rad(double deg) {
    return (deg * pi) / 180.0;
}


void SeedRng(unsigned int seed)
{
    srand(seed);
}

void SeedRng()
{
    SeedRng((unsigned int)time(nullptr));
}

//inline double random_double() {
//    return rand() / (RAND_MAX + 1.0);
//}
//
//inline double random_double(double min, double max) {
//    return min + (max - min) * random_double();
//}

// use newer c++ features
inline double random_double() {
    static random_device rd;
    static mt19937 generator(rd());
    static uniform_real_distribution<double> distribution(0.0, 1.0);
    return distribution(generator);
}

inline double random_double(double min, double max) {
    static random_device rd;
    static mt19937 generator(rd());
    static uniform_real_distribution<double> distribution(min, max);
    return distribution(generator);
}

inline int random_int() {
    return int(random_double(0, 2));
}

inline int random_int(int min, int max) {
    return int(random_double(min, max+1));
}

#include "color.h"
#include "interval.h"
#include "ray.h"
#include "vec3.h"

#endif //GRAPHICA_CONSTANTS_H
