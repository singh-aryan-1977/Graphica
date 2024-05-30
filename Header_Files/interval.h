//
// Created by Aryan Singh on 5/29/24.
//

#ifndef GRAPHICA_INTERVAL_H
#define GRAPHICA_INTERVAL_H

#include "constants.h"

class interval {
public:
    double min, max;

    interval(): min(+inf), max(-inf) {}

    interval(double min, double max): min(min), max(max) {}

    double size() const {
        return max - min;
    }

    bool contains(double x) const {
        return min <= x && x <= max;
    }

    bool surrounds(double x) const {
        return min < x && x < max;
    }

    static const interval empty, universe;
};

const interval interval::empty = interval();
const interval interval::universe = interval(-inf, inf);

#endif //GRAPHICA_INTERVAL_H
