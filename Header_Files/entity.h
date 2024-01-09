//
// Created by Aryan Singh on 1/8/24.
//

#ifndef GRAPHICA_ENTITY_H
#define GRAPHICA_ENTITY_H
#include "ray.h"

class entity_record {
public:
    point3 p;
    vec3 normal;
    double t{};
    bool front_face;

    void set_face_normal(const ray& r, const vec3& outward_normal) {
        // Checking if ray is inside object or outisde object
        front_face = dot(r.direction(), outward_normal) < 0;
        normal = front_face ? outward_normal : -outward_normal;
    }
};

class entity {
public:
    virtual ~entity() = default;
    virtual bool hit(const ray& r, double ray_tmin, double ray_tmax, entity_record& rec) const=0;
};

#endif //GRAPHICA_ENTITY_H