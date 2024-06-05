//
// Created by Aryan Singh on 6/4/24.
//

#ifndef GRAPHICA_MATERIAL_H
#define GRAPHICA_MATERIAL_H
#include "constants.h"

class entity_record;

class material {
    public:
        virtual ~material() = default;

        virtual bool scatter(const ray& incidence, const entity_record& record, color& change, ray& scattered) const {
            return false;
        }
};

class lambertian: public material {
    public:
        lambertian(const color& albedo) : albedo(albedo) {};

        bool scatter(const ray& incidence, const entity_record& record, color& change, ray& scattered)
        const override {
            auto scattered_direction = record.normal + normalize_vec_in_unit_sphere();
            if (scattered_direction.is_near_zero()) {
                scattered_direction = record.normal;
            }
            scattered = ray(record.p, scattered_direction);
            change = albedo;
            return true;
        }

    private:
        color albedo;
};

class metal: public material {
public:
    metal(const color& albedo) : albedo(albedo) {};

    bool scatter(const ray& incidence, const entity_record& record, color& change, ray& scattered)
    const override {
        vec3 reflected_ray = metal_reflect(incidence.direction(), record.normal);
        scattered = ray(record.p, reflected_ray);
        change = albedo;
        return true;
    }

private:
    color albedo;
};

class dielectric: public material {
public:
    dielectric(double refractive_index) : refractive_index(refractive_index) {};

    bool scatter(const ray& incidence, const entity_record& record, color& change, ray& scattered)
    const override {
//        vec3 reflected_ray = metal_reflect(incidence.direction(), record.normal);
//        scattered = ray(record.p, reflected_ray);
        double refractive_ratio;
        if (record.front_face) {
            refractive_ratio = 1.0 / refractive_index;
        } else {
            refractive_ratio = refractive_index;
        }
        // cos(theta) = R * normal
        double theta_cos = fmin(dot(-unit_vector(incidence.direction()), record.normal), 1.0);
        double theta_sin = sqrt(1-theta_cos*theta_cos); // sin(theta) = sqrt(1-cos^2(theta))

        bool can_refract = refractive_ratio * theta_sin < 1.0; //  have to check for total internal reflection
        vec3 returned_ray;
        if (can_refract) {
            returned_ray = refract(unit_vector(incidence.direction()), record.normal, refractive_ratio);
        } else {
            returned_ray = metal_reflect(unit_vector(incidence.direction()),record.normal);
        }
        change = color(1.0, 1.0, 1.0);
        scattered = ray(record.p, returned_ray);
        return true;
    }

private:
    double refractive_index;
};
#endif //GRAPHICA_MATERIAL_H
