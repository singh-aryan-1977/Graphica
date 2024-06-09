//
// Created by Aryan Singh on 1/8/24.
//

#ifndef GRAPHICA_SPHERE_H
#define GRAPHICA_SPHERE_H
#include "entity.h"
#include "vec3.h"
class sphere: public entity {
public:
    sphere(const point3& _center, double _radius, shared_ptr<material> materials): center(_center),
    radius(fmax(0, _radius)), materials(materials), is_moving(false),
    center_vector(vec3(0,0,0)) {}

    sphere(const point3& _center, const point3& _new_center, double _radius, shared_ptr<material> materials): center(_center),
    radius(fmax(0, _radius)), materials(materials), is_moving(true)
    {
        center_vector = _new_center - _center;
    }

    bool hit(const ray& r, interval ray_t, entity_record& rec) const override{
        point3 curr_center = is_moving ? new_center(r.time()) : center;
        vec3 dist = r.origin()-curr_center;
//        vec3 dist = curr_center - r.origin();
        auto a = r.direction().length_squared();
        auto half_b = dot(dist, r.direction());
        auto c = dist.length_squared() - (radius*radius);
        auto d = (half_b*half_b)-(a*c);
        if (d < 0){
            return false;
        }
        auto root = (-half_b-sqrt(d)) / a;

        // finding nearest root in range of ray_tmin to ray_tmax
        if (root <= ray_t.min || root >= ray_t.max) {
            root = (-half_b+sqrt(d)) / a;
            if (root <= ray_t.min || root >= ray_t.max) {
                return false;
            }
        }

        // setting entity-record values
        rec.t = root;
        rec.p = r.at(rec.t);
        vec3 outward_normal = (rec.p-center) / radius;
        rec.set_face_normal(r, outward_normal);
        rec.materials = materials;
        return true;
    }
private:
    point3 center;
    double radius;
    shared_ptr<material> materials;
    vec3 center_vector;
    bool is_moving = false;

    point3 new_center(double time) const {
        return center + time * center_vector;
    }
};
#endif //GRAPHICA_SPHERE_H
