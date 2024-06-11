//
// Created by Aryan Singh on 6/10/24.
//

#ifndef GRAPHICA_QUADRILATERAL_H
#define GRAPHICA_QUADRILATERAL_H

#include "constants.h"
#include "entity.h"
#include "entity_list.h"

class quadrilateral : public entity {
public:

    // Q is lower left corner, u and v are vectors adjacent to Q
    quadrilateral(const point3& q, const vec3& u, const vec3& v, shared_ptr<material> materials) :
    q(q), u(u), v(v), materials(materials) {
        normal = unit_vector(cross(u, v));
        D = dot(normal, q); // for plane equation for quadrilateral (Ax+By+Cz = D)
        initialize();
        w = cross(u, v) / dot(cross(u, v), cross(u, v));
        area = cross(u, v).length();
    }

    virtual void set_bounding_box() {
        auto bbox_diagonal_to_top_right = axis_aligned_bounding_box(q, q+u+v);
        auto bbox_diagonal2 = axis_aligned_bounding_box(q+u, q+v);
        bbox = axis_aligned_bounding_box(bbox_diagonal_to_top_right, bbox_diagonal2);
    }

    [[nodiscard]] axis_aligned_bounding_box bounding_box() const override {
        return bbox;
    }

    bool hit(const ray& incidence, interval ray, entity_record& record) const override {
        auto denominator = dot(normal, incidence.direction());

        if (fabs(denominator) < 1e-8) {
            return false; // parallel ray to plane
        }

        // t = (D - n*P) / n * d  remember: ray: R(t) = P + td
        auto t = (D - dot(normal, incidence.origin())) / denominator;
        if (!ray.contains(t)) {
            return false; // no intersection
        }

        auto intersection_point = incidence.at(t);
        vec3 planar_vector = intersection_point-q;
        auto alpha = dot(w, cross(planar_vector, v));
        auto beta = dot(w, cross(u, planar_vector));

        if (!interior(alpha, beta, record)) {
            return false;
        }
        record.t = t;
        record.p = intersection_point;
        record.materials = materials;
        record.set_face_normal(incidence, normal);
        return true;


        return false;
    }

    virtual bool interior(double alpha, double beta, entity_record& record) const {
        interval unit = interval(0, 1);

        if (!unit.contains(alpha) || !unit.contains(beta)) {
            return false;
        }
        record.u = alpha;
        record.v = beta;
        return true;
    }

    double pdf_value(const point3& origin, const point3& direction) const override {
        entity_record record;
        auto epsilon = 0.001;
        if (!this->hit(ray(origin, direction), interval(epsilon, inf), record)) {
            return 0.0;
        }

        auto dist_squared = record.t * record.t * direction.length_squared();
        auto cos_theta = fabs(dot(direction, record.normal) / direction.length());
        return dist_squared / (cos_theta * area);
    }

     vec3 random(const point3& origin) const override {
        auto p = q + (random_double() * u) + (random_double() * v);
        return p-origin;
    }
private:
    point3 q;
    vec3 u,v;
    shared_ptr<material> materials;
    axis_aligned_bounding_box bbox;
    double D;
    vec3 normal;
    vec3 w;
    double area;

    void initialize() {
        set_bounding_box();
    }
};

inline shared_ptr<entity_list> box(const point3& a, const point3& b, shared_ptr<material> materials) {
    auto sides = make_shared<entity_list>();

    auto min = point3(fmin(a.x(), b.x()), fmin(a.y(), b.y()), fmin(a.z(), b.z()));
    auto max = point3(fmax(a.x(), b.x()), fmax(a.y(), b.y()), fmax(a.z(), b.z()));

    auto dx = vec3(max.x() - min.x(), 0, 0);
    auto dy = vec3(0, max.y() - min.y(), 0);
    auto dz = vec3(0, 0, max.z() - min.z());

    // front face
    sides->add(make_shared<quadrilateral>(point3(min.x(), min.y(), max.z()), dx, dy, materials));
    // back face
    sides->add(make_shared<quadrilateral>(point3(max.x(), min.y(), max.z()), -dx, dy, materials));
    // left face
    sides->add(make_shared<quadrilateral>(point3(min.x(), min.y(), min.z()), dz, dy, materials));
    // right face
    sides->add(make_shared<quadrilateral>(point3(max.x(), min.y(), max.z()), -dz, dy, materials));
    // top face
    sides->add(make_shared<quadrilateral>(point3(min.x(), max.y(), max.z()), dx, -dz, materials));
    // bottom face
    sides->add(make_shared<quadrilateral>(point3(min.x(), min.y(), min.z()), dx, dz, materials));

    return sides;
}

#endif //GRAPHICA_QUADRILATERAL_H
