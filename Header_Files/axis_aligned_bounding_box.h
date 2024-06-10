//
// Created by Aryan Singh on 6/8/24.
//

#ifndef GRAPHICA_AXIS_ALIGNED_BOUNDING_BOX_H
#define GRAPHICA_AXIS_ALIGNED_BOUNDING_BOX_H

#include "constants.h"

class axis_aligned_bounding_box {
public:
    interval x,y,z;

    axis_aligned_bounding_box() {};

    axis_aligned_bounding_box(const interval& x, const interval& y, const interval &z): x(x), y(y), z(z) {}

    // apparently fmin and fmax are slower than simple conditional???
    // https://stackoverflow.com/questions/76387817/fmin-and-fmax-are-much-slower-than-simple-conditional-operator
    axis_aligned_bounding_box(const point3& a, const point3& b) {
        x = (a[0] <= b[0]) ? interval(a[0], b[0]) : interval(b[0], a[0]);
        y = (a[1] <= b[1]) ? interval(a[1], b[1]) : interval(b[1], a[1]);
        z = (a[2] <= b[2]) ? interval(a[2], b[2]) : interval(b[2], a[2]);
    }

    axis_aligned_bounding_box(const axis_aligned_bounding_box& box1, const axis_aligned_bounding_box& box2) {
        x = interval(box1.x, box2.x);
        y = interval(box1.y, box2.y);
        z = interval(box1.z, box2.z);
    }

    const interval& axis_of_interval(int n) const {
        if (n == 0) {
            return x;
        } else if (n == 1) {
            return y;
        } else {
            return z;
        }
    }

    bool hit(const ray& r, interval ray) const {
        const point3& origin = r.origin();
        const vec3& direction = r.direction();
        for (int a = 0; a < 3; a++) {
            const interval& axis = axis_of_interval(a);
            const double inverse = 1.0 / direction[a];

            auto t0 = (axis.min - origin[a]) * inverse;
            auto t1 = (axis.max - origin[a]) * inverse;

            if (t0 < t1) {
                if (t0 > ray.min) {
                    ray.min = t0;
                }
                if (t1 < ray.max) {
                    ray.max = t1;
                }
            } else {
                if (t1 > ray.min) {
                    ray.min = t1;
                }
                if (t0 < ray.max) {
                    ray.max = t0;
                }
            }

            if (ray.max <= ray.min) {
                return false;
            }

        }
        return true;
    }

    int longest_axis() const {
        if (x.size() > y.size()) {
            if (x.size() > z.size()) {
                return 0;
            } else {
                return 2;
            }
        } else {
            if (y.size() > z.size()) {
                return 1;
            } else {
                return 2;
            }
        }
    }

    static const axis_aligned_bounding_box empty, universe;
};

const axis_aligned_bounding_box axis_aligned_bounding_box::empty = axis_aligned_bounding_box(interval::empty, interval::empty, interval::empty);
const axis_aligned_bounding_box axis_aligned_bounding_box::universe = axis_aligned_bounding_box(interval::universe, interval::universe, interval::universe);
#endif //GRAPHICA_AXIS_ALIGNED_BOUNDING_BOX_H
