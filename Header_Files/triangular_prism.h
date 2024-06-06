#ifndef GRAPHICA_TRIANGULAR_PRISM_H
#define GRAPHICA_TRIANGULAR_PRISM_H

#include "entity.h"
#include "vec3.h"

class triangular_prism : public entity {
public:
    triangular_prism(const point3& v0, const point3& v1, const point3& v2, shared_ptr<material> mat)
            : v0(v0), v1(v1), v2(v2), materials(mat) {}

    virtual bool hit(const ray& r, interval ray_t, entity_record& rec) const override {
        const double EPSILON = 0.0000001;

        for (int i = 0; i < 6; i += 3) {
//            const point3& v0 = vertices[i];
//            const point3& v1 = vertices[i + 1];
//            const point3& v2 = vertices[i + 2];

            // Calculate normal of the triangle
            vec3 edge1 = v1 - v0;
            vec3 edge2 = v2 - v0;
            vec3 normal = cross(edge1, edge2);

            // Check if ray is parallel to the triangle
            double den = dot(normal, r.direction());
            if (fabs(den) < EPSILON) {
                continue;
            }

            // Calculate intersection point
            vec3 v0_to_ray_origin = r.origin() - v0;
            double t = -dot(normal, v0_to_ray_origin) / den;
            if (t < ray_t.min || t > ray_t.max) {
                continue;
            }

            point3 intersection_point = r.at(t);

            // Check if intersection point is inside the triangle
            vec3 C, edge0;
            bool inside = true;

            edge0 = v1 - v0;
            C = cross(edge0, intersection_point - v0);
            if (dot(normal, C) < 0) {
                inside = false;
            }

            edge0 = v2 - v1;
            C = cross(edge0, intersection_point - v1);
            if (dot(normal, C) < 0) {
                inside = false;
            }

            edge0 = v0 - v2;
            C = cross(edge0, intersection_point - v2);
            if (dot(normal, C) < 0) {
                inside = false;
            }

            if (!inside) {
                continue;
            }

            // Intersection found, fill the entity record
            rec.t = t;
            rec.p = intersection_point;
            rec.set_face_normal(r, normal);
            rec.materials = materials;
            return true;
        }

        return false;
    }


private:
    point3 v0, v1, v2; // Vertices of the triangular prism
    shared_ptr<material> materials; // Material of the prism
};

#endif //GRAPHICA_TRIANGULAR_PRISM_H
