//
// Created by Aryan Singh on 1/7/24.
//

#ifndef GRAPHICA_VEC3_H
#define GRAPHICA_VEC3_H

#include <cmath>
#include <iostream>


using std::sqrt;

class vec3 {
public:
    double e[3];

    vec3() : e{0,0,0} {}
    vec3(double e0, double e1, double e2) : e{e0, e1, e2} {}

    double x() const { return e[0]; }
    double y() const { return e[1]; }
    double z() const { return e[2]; }

    vec3 operator-() const { return vec3(-e[0], -e[1], -e[2]); }
    double operator[](int i) const { return e[i]; }
    double& operator[](int i) { return e[i]; }

    vec3& operator+=(const vec3 &v) {
        e[0] += v.e[0];
        e[1] += v.e[1];
        e[2] += v.e[2];
        return *this;
    }

    vec3& operator*=(double t) {
        e[0] *= t;
        e[1] *= t;
        e[2] *= t;
        return *this;
    }

    vec3& operator/=(double t) {
        return *this *= 1/t;
    }

    double length() const {
        return sqrt(length_squared());
    }

    double length_squared() const {
        return e[0]*e[0] + e[1]*e[1] + e[2]*e[2];
    }

    static vec3 random_vector(double min, double max) {
        return vec3(random_double(min, max), random_double(min, max), random_double(min, max));
    }

    bool is_near_zero() const {
        auto epsilon = 1e-8;
        return (fabs(e[0]) < epsilon) && (fabs(e[1]) < epsilon) &&  (fabs(e[2]) < epsilon);
    }
};

// point3 is just an alias for vec3, but useful for geometric clarity in the code.
using point3 = vec3;


// Vector Utility Functions

inline std::ostream& operator<<(std::ostream &out, const vec3 &v) {
    return out << v.e[0] << ' ' << v.e[1] << ' ' << v.e[2];
}

inline vec3 operator+(const vec3 &u, const vec3 &v) {
    return vec3(u.e[0] + v.e[0], u.e[1] + v.e[1], u.e[2] + v.e[2]);
}

inline vec3 operator-(const vec3 &u, const vec3 &v) {
    return vec3(u.e[0] - v.e[0], u.e[1] - v.e[1], u.e[2] - v.e[2]);
}

inline vec3 operator*(const vec3 &u, const vec3 &v) {
    return vec3(u.e[0] * v.e[0], u.e[1] * v.e[1], u.e[2] * v.e[2]);
}

inline vec3 operator*(double t, const vec3 &v) {
    return vec3(t*v.e[0], t*v.e[1], t*v.e[2]);
}

inline vec3 operator*(const vec3 &v, double t) {
    return t * v;
}

inline vec3 operator/(vec3 v, double t) {
    return (1/t) * v;
}

inline double dot(const vec3 &u, const vec3 &v) {
    return u.e[0] * v.e[0]
           + u.e[1] * v.e[1]
           + u.e[2] * v.e[2];
}

inline vec3 cross(const vec3 &u, const vec3 &v) {
    return vec3(u.e[1] * v.e[2] - u.e[2] * v.e[1],
                u.e[2] * v.e[0] - u.e[0] * v.e[2],
                u.e[0] * v.e[1] - u.e[1] * v.e[0]);
}

inline vec3 unit_vector(vec3 v) {
    return v / v.length();
}

inline vec3 present_in_unit_sphere() {
    while (true) {
        auto sampled_vector = vec3::random_vector(-1, 1);
        if (sampled_vector.length_squared() < 1) {
            return sampled_vector;
        }
    }
}

inline vec3 normalize_vec_in_unit_sphere() {
    return unit_vector(present_in_unit_sphere());
}

inline vec3 check_orientation(const vec3& normal) {
    vec3 unit_sphere_vector = normalize_vec_in_unit_sphere();
    if (dot(unit_sphere_vector, normal) > 0.0) {
        return unit_sphere_vector;
    } else {
        return -unit_sphere_vector;
    }
}

inline vec3 metal_reflect(const vec3& incidence, const vec3& normal) {
    return incidence - 2 * dot(incidence, normal) * normal;
}

// use snell's law
inline vec3 refract(const vec3& refracted, const vec3& normal, double refractive_ratio) {
    auto cos = fmin(dot(refracted, normal), 0);
    vec3 refracted_perpendicular_component =
            refractive_ratio * (refracted + cos * normal); // R_perp = n/n' * (r + cos (theta) * n)
    // r_parallel = -sqrt(1-r_perp^2) * n
    vec3 refracted_parallel_component = -sqrt(fabs(1 - refracted_perpendicular_component.length_squared())) * normal;
    return refracted_perpendicular_component + refracted_parallel_component;
}

#endif //GRAPHICA_VEC3_H
