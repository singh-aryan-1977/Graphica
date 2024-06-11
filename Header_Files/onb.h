//
// Created by Aryan Singh on 6/10/24.
//

// orthonormal basis (making our own relative coordinate system)

#ifndef GRAPHICA_ONB_H
#define GRAPHICA_ONB_H

#include "constants.h"

class onb {
public:
    onb() {}

    vec3 operator[](int i) const { return axis[i]; }
    vec3& operator[](int i) { return axis[i]; }

    vec3 u() const { return axis[0]; }
    vec3 v() const { return axis[1]; }
    vec3 w() const { return axis[2]; }

    vec3 local(double a, double b, double c) const {
        return a*u() + b*v() + c*w();
    }

    vec3 local(const vec3& vec) const {
        return vec.x()*u() + vec.y()*v() + vec.z()*w();
    }

    void build(const vec3& w_basis) {
        vec3 unit = unit_vector(w_basis);
        axis[2] = unit;

        // check if parallel to x-axis, if so use y-axis otherwise stick with x
        vec3 non_parallel = (fabs(unit.x()) > 0.9) ? vec3(0,1,0) : vec3(1,0,0);
        vec3 v = unit_vector(cross(unit, non_parallel));
        axis[0] = cross(unit, v);
        axis[1] = v;
    }
private:
    vec3 axis[3];
};

#endif //GRAPHICA_ONB_H
