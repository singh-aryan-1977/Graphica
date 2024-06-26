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

    interval(const interval& a, const interval& b) {
        min = fmin(a.min, b.min);
        max = fmax(a.max, b.max);
    }

    double size() const {
        return max - min;
    }

    bool contains(double x) const {
        return min <= x && x <= max;
    }

    bool surrounds(double x) const {
        return min < x && x < max;
    }

    double bound_to(double x) const {
        if (x < min) {
            return min;
        }

        if (x > max) {
            return max;
        }
        return x;
    }

    [[nodiscard]] interval pad(double x) const {
        return interval(min-x/2, max+x/2);
    }




    static const interval empty, universe;
};

const interval interval::empty = interval();
const interval interval::universe = interval(-inf, inf);

interval operator+(const interval& inter, double value) {
    return interval(inter.min+value, inter.max+value);
}

interval operator+(double value, const interval& inter) {
    return inter+value;
}



#endif //GRAPHICA_INTERVAL_H
