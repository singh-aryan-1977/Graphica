//
// Created by Aryan Singh on 6/8/24.
//

#ifndef GRAPHICA_PYRAMID_H
#define GRAPHICA_PYRAMID_H

#include "entity.h"
#include "vec3.h"
#include <array>

class pyramid : public entity {
public:
    pyramid(const point3& _v0, const point3& _v1, const point3& _v2, const point3& _v3, const point3& _apex, shared_ptr<material> mat)
            : base{_v0, _v1, _v2, _v3}, apex(_apex), materials(mat) {}

    bool hit(const ray& r, interval ray_t, entity_record& rec) const override {
        float t;
        bool hit_anything = false;
        float closest_so_far = ray_t.max;

        // Check base face
        if (rayTriangleIntersect(r.origin(), r.direction(), base[0], base[1], base[2], t)) {
            if (t < closest_so_far && t > ray_t.min) {
                closest_so_far = t;
                hit_anything = true;
                rec.t = t;
                rec.p = r.at(t);
                vec3 outward_normal = unit_vector(cross(base[1] - base[0], base[2] - base[0]));
                rec.set_face_normal(r, outward_normal);
                rec.materials = materials;
            }
        }
        if (rayTriangleIntersect(r.origin(), r.direction(), base[2], base[3], base[0], t)) {
            if (t < closest_so_far && t > ray_t.min) {
                closest_so_far = t;
                hit_anything = true;
                rec.t = t;
                rec.p = r.at(t);
                vec3 outward_normal = unit_vector(cross(base[3] - base[2], base[0] - base[2]));
                rec.set_face_normal(r, outward_normal);
                rec.materials = materials;
            }
        }

        // Check side faces
        for (int i = 0; i < 4; ++i) {
            if (rayTriangleIntersect(r.origin(), r.direction(), base[i], base[(i+1)%4], apex, t)) {
                if (t < closest_so_far && t > ray_t.min) {
                    closest_so_far = t;
                    hit_anything = true;
                    rec.t = t;
                    rec.p = r.at(t);
                    vec3 outward_normal = unit_vector(cross(base[(i+1)%4] - base[i], apex - base[i]));
                    rec.set_face_normal(r, outward_normal);
                    rec.materials = materials;
                }
            }
        }

        return hit_anything;
    }

private:
    std::array<point3, 4> base;
    point3 apex;
    shared_ptr<material> materials;

    bool rayTriangleIntersect(
            const vec3& orig, const vec3& dir,
            const vec3& v0, const vec3& v1, const vec3& v2,
            float& t) const
    {
        vec3 v0v1 = v1 - v0;
        vec3 v0v2 = v2 - v0;
        vec3 N = cross(v0v1, v0v2);
        float area2 = N.length();

        float NdotRayDirection = dot(N, dir);
        if (fabs(NdotRayDirection) < 1e-8)
            return false;

        float d = -dot(N, v0);
        t = -(dot(N, orig) + d) / NdotRayDirection;
        if (t < 0) return false;

        vec3 P = orig + t * dir;
        vec3 C;

        vec3 edge0 = v1 - v0;
        vec3 vp0 = P - v0;
        C = cross(edge0, vp0);
        if (dot(N, C) < 0) return false;

        vec3 edge1 = v2 - v1;
        vec3 vp1 = P - v1;
        C = cross(edge1, vp1);
        if (dot(N, C) < 0) return false;

        vec3 edge2 = v0 - v2;
        vec3 vp2 = P - v2;
        C = cross(edge2, vp2);
        if (dot(N, C) < 0) return false;

        return true;
    }
};

#endif // GRAPHICA_PYRAMID_H
