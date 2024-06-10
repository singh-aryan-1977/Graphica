//
// Created by Aryan Singh on 6/10/24.
//

#ifndef GRAPHICA_VOLUMES_H
#define GRAPHICA_VOLUMES_H

#include "entity.h"
#include "constants.h"
#include "texture.h"
#include "material.h"

class volumes : public entity {
public:
    volumes(shared_ptr<entity> boundary, double density, shared_ptr<texture> textures)
    : boundary(boundary), neg_density(-1.0/density), phase_function(make_shared<isotropic>(textures)) {}

    volumes(shared_ptr<entity> boundary, double density, const color& albedo)
    : boundary(boundary), neg_density(-1.0/density), phase_function(make_shared<isotropic>(albedo)) {}

    [[nodiscard]] axis_aligned_bounding_box bounding_box() const override {
        return boundary->bounding_box();
    }

    bool hit(const ray& r, interval ray_t, entity_record& rec) const override {
        entity_record record1, record2;

        if (!boundary->hit(r, interval::universe, record1)) {
            return false;
        }

        auto epsilon = 0.0001;

        if (!boundary->hit(r, interval(record1.t + epsilon, inf), record2)) {
            return false;
        }

        // do simple conditional operator for faster time since fmin is slower
        if (record1.t < ray_t.min) {
//            ray_t.min = record1.t;
            record1.t = ray_t.min;
        }
        if (record2.t > ray_t.max) {
//            ray_t.max = record2.t;
            record2.t = ray_t.max;

        }

        if (record2.t <= record1.t) {
            return false;
        }

        if (record1.t < 0) {
            record1.t = 0;
        }

        auto length = r.direction().length();

        auto dist_in_boundary = (record2.t-record1.t) * length;
        auto hit_distance = neg_density * log(random_double()); // probability = C (neg_density) * delta(L)
        if (hit_distance > dist_in_boundary) {
            return false; // outside of boundary
        }

        rec.t = record1.t + hit_distance / length;
        rec.p = r.at(rec.t);

        rec.normal = vec3(1,0,0);
        rec.front_face = true;
        rec.materials = phase_function;

        return true;
    }

private:
    shared_ptr<entity> boundary;
    double neg_density;
    shared_ptr<material> phase_function;
};

#endif //GRAPHICA_VOLUMES_H
