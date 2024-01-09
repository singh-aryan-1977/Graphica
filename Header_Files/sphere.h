//
// Created by Aryan Singh on 1/8/24.
//

#ifndef GRAPHICA_SPHERE_H
#define GRAPHICA_SPHERE_H
#include "entity.h"
#include "vec3.h"
class sphere: public entity {
public:
    sphere(point3 _center, double _radius): center(_center), radius(_radius) {}

    bool hit(const ray& r, double ray_tmin, double ray_tmax, entity_record& rec) const override{
        vec3 dist = r.origin()-center;
        auto a = r.direction().length_squared();
        auto half_b = dot(dist, r.direction());
        auto c = dist.length_squared() - (radius*radius);
        auto d = (half_b*half_b)-(a*c);
        if (d < 0){
            return false;
        }
        auto root = (-half_b-sqrt(d)) / a;

        // finding nearest root in range of ray_tmin to ray_tmax
        if (root <= ray_tmin || root >= ray_tmax) {
            root = (-half_b+sqrt(d)) / a;
            if (root <= ray_tmin || root >= ray_tmax) {
                return false;
            }
        }

        // setting entity-record values
        rec.t = root;
        rec.p = r.at(rec.t);
        vec3 outward_normal = (rec.p-center) / radius;
        rec.set_face_normal(r, outward_normal);
        return true;
    }
private:
    point3 center;
    double radius;
};
#endif //GRAPHICA_SPHERE_H
