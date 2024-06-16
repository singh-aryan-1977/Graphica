//
// Created by Aryan Singh on 1/8/24.
//

#ifndef GRAPHICA_ENTITY_H
#define GRAPHICA_ENTITY_H
#include "ray.h"
#include "interval.h"
#include "axis_aligned_bounding_box.h"



class material;

class entity_record {
public:
    point3 p;
    vec3 normal;
    double t{};
    bool front_face;
    shared_ptr<material> materials;
    double u,v;

    void set_face_normal(const ray& r, const vec3& outward_normal) {
        // Checking if ray is inside object or outisde object
        front_face = dot(r.direction(), outward_normal) < 0;
        normal = front_face ? outward_normal : -outward_normal;
    }
};

class entity {
public:
    virtual ~entity() = default;
    virtual bool hit(const ray& r, interval ray_t, entity_record& rec) const=0;
    [[nodiscard]] virtual axis_aligned_bounding_box bounding_box() const = 0;
    virtual double pdf_value(const point3& origin, const vec3& direction) const {
        return 0.0;
    }

    virtual vec3 random(const point3& origin) const {
        return vec3(1, 0, 0);
    }

};

// move ray back by offset, check intersection, move ray towards offset again
class translate : public entity {
public:

    translate(shared_ptr<entity> obj, const vec3& offset) : obj(obj), offset(offset) {
        bbox = obj->bounding_box() + offset;
    }

    bool hit(const ray& r, interval ray_t, entity_record& rec) const override {
        // move ray back by offset
        ray offset_back(r.origin() - offset, r.direction(), r.time());

        // check intersection
        if (!obj->hit(offset_back, ray_t, rec)) {
            return false;
        }

        // move ray towards offset again
        rec.p += offset;
        return true;
    }

    [[nodiscard]] axis_aligned_bounding_box bounding_box() const override {
        return bbox;
    }
private:
    vec3 offset;
    shared_ptr<entity> obj;
    axis_aligned_bounding_box bbox;
};

class rotate_y : public entity {
public:

    rotate_y(shared_ptr<entity> obj, double angle) : obj(obj) {
        auto radians = deg_to_rad(angle);
        cos_theta = cos(radians);
        sin_theta = sin(radians);
        bbox = obj->bounding_box();

        // get rotated bounding box
        point3 max(-inf, -inf, -inf);
        point3 min(inf, inf, inf);

        for (int i = 0; i < 2; i++) {
            for (int j = 0; j < 2; j++) {
                for (int k = 0; k < 2; k++) {
                    auto x = i * bbox.x.max + (i-1) * bbox.x.min;
                    auto y = j * bbox.y.max + (j-1) * bbox.y.min;
                    auto z = k * bbox.z.max + (k-1) * bbox.z.min;

                    auto new_x = cos_theta*x + sin_theta*z;
                    auto new_z = -sin_theta*x + cos_theta*z;

                    vec3 check(new_x, y, new_z);

                    for (int component = 0; component < 2; component++) {
                        min[component] = fmin(min[component], check[component]);
                        max[component] = fmax(max[component], check[component]);
                    }
                }
            }
        }

        bbox = axis_aligned_bounding_box(min, max);

    }
    bool hit(const ray& r, interval ray_t, entity_record& rec) const override {
        auto origin = r.origin();
        auto dir = r.direction();

        auto original_origin = origin;
        auto original_dir = dir;

        // rotate to new coords
        origin[0] = cos_theta * original_origin[0] - sin_theta * original_origin[2];
        origin[2] = sin_theta * original_origin[0] + cos_theta * original_origin[2];

        dir[0] = cos_theta * original_dir[0] - sin_theta * original_dir[2];
        dir[2] = sin_theta * original_dir[0] + cos_theta * original_dir[2];

        ray rotated_ray(origin, dir, r.time());

        if (!obj->hit(rotated_ray, ray_t, rec)) {
            return false; // no intersection
        }

        // rotate back
        auto p = rec.p;
        auto original_p = p;
        p[0] = cos_theta * original_p[0] + sin_theta * original_p[2];
        p[2] = -sin_theta * original_p[0] + cos_theta * original_p[2];

        // change normal back
        auto n = rec.normal;
        auto original_normal = n;
        n[0] = cos_theta * original_normal[0] + sin_theta * original_normal[2];
        n[2] = -sin_theta * original_normal[0] + cos_theta * original_normal[2];

        rec.p = p;
        rec.normal = n;
        return true;
    }

    axis_aligned_bounding_box bounding_box() const override {
        return bbox;
    }
private:
    double cos_theta, sin_theta;
    axis_aligned_bounding_box bbox;
    shared_ptr<entity> obj;
};

#endif //GRAPHICA_ENTITY_H
