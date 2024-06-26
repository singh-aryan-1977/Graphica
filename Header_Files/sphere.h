//
// Created by Aryan Singh on 1/8/24.
//

#ifndef GRAPHICA_SPHERE_H
#define GRAPHICA_SPHERE_H
#include "entity.h"
#include "vec3.h"
#include "onb.h"
class sphere: public entity {
public:
    sphere(const point3& _center, double _radius, shared_ptr<material> materials): center(_center),
    radius(fmax(0, _radius)), materials(materials), is_moving(false)
    {
        auto radius_vector = vec3(radius, radius, radius);
        bbox = axis_aligned_bounding_box(center - radius_vector, center + radius_vector);
    }

    sphere(const point3& _center, const point3& _new_center, double _radius, shared_ptr<material> materials): center(_center),
    radius(fmax(0, _radius)), materials(std::move(materials)), is_moving(true)
    {
        center_vector = _new_center - _center;
        auto radius_vector = vec3(radius, radius, radius);
        axis_aligned_bounding_box box1(_center - radius_vector, _center + radius_vector);
        axis_aligned_bounding_box box2(_new_center - radius_vector, _new_center + radius_vector);
        bbox = axis_aligned_bounding_box(box1, box2);

    }

    [[nodiscard]] axis_aligned_bounding_box bounding_box() const override {return bbox;}

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
        if (!ray_t.surrounds(root)) {
            root = (-half_b + sqrt(d)) / a;
            if (!ray_t.surrounds(root)) {
                return false;
            }
        }

        // setting entity-record values
        rec.t = root;
        rec.p = r.at(rec.t);
        vec3 outward_normal = (rec.p-center) / radius;
        rec.set_face_normal(r, outward_normal);
        get_sphere_uv_coord(outward_normal, rec.u, rec.v);
        rec.materials = materials;
        return true;
    }

    double pdf_value(const point3& origin, const vec3& direction) const override {
        // This method only works for stationary spheres.

        entity_record rec;
        if (!this->hit(ray(origin, direction), interval(0.001, inf), rec))
            return 0;

        auto cos_theta_max = sqrt(1 - radius*radius/(center - origin).length_squared());
        auto solid_angle = 2*pi*(1-cos_theta_max);

        return  1 / solid_angle;
    }

    vec3 random(const point3& origin) const override {
        vec3 direction = center - origin;
        auto distance_squared = direction.length_squared();
        onb uvw;
        uvw.build(direction);
        return uvw.local(random_to_sphere(radius, distance_squared));
    }
private:
    point3 center;
    double radius;
    shared_ptr<material> materials;
    vec3 center_vector;
    bool is_moving = false;
    axis_aligned_bounding_box bbox;

    point3 new_center(double time) const {
        return center + time * center_vector;
    }

    static void get_sphere_uv_coord(const point3& p, double &u, double &v) {
        // p is point on unit sphere centered on origin
        // u is phi (cut through y-axis angle)
        // v is theta (sweep across y-axis angle)
        // this is basically polar coordinate conversion

        auto theta = acos(-p.y());
        auto phi = atan2(-p.z(), p.x()) + pi;

        u = phi / (2*pi);
        v = theta/pi;
    }

    static vec3 random_to_sphere(double radius, double distance_squared) {
        auto r1 = random_double();
        auto r2 = random_double();
        auto z = 1 + r2*(sqrt(1-radius*radius/distance_squared) - 1);

        auto phi = 2*pi*r1;
        auto x = cos(phi)*sqrt(1-z*z);
        auto y = sin(phi)*sqrt(1-z*z);

        return vec3(x, y, z);
    }
};
#endif //GRAPHICA_SPHERE_H
