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
    metal(const color& albedo, double fuzz) : albedo(albedo), fuzz(fuzz < 1.0 ? fuzz : 1) {};

    bool scatter(const ray& incidence, const entity_record& record, color& change, ray& scattered)
    const override {
        vec3 reflected_ray = metal_reflect(incidence.direction(), record.normal);
        reflected_ray = unit_vector(reflected_ray) + (fuzz * normalize_vec_in_unit_sphere());
        scattered = ray(record.p, reflected_ray);
        change = albedo;
        return (dot(scattered.direction(), record.normal) > 0.0);
    }

private:
    color albedo;
    double fuzz;
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
            refractive_ratio = 1.0/refractive_index;
        } else {
            refractive_ratio = refractive_index;
        }
        // cos(theta) = R * normal
        double theta_cos = fmin(dot(-unit_vector(incidence.direction()), record.normal), 1.0);
        double theta_sin = sqrt(1.0-theta_cos*theta_cos); // sin(theta) = sqrt(1-cos^2(theta))

        bool can_refract = refractive_ratio * theta_sin < 1.0; //  have to check for total internal reflection
        vec3 returned_ray;
        if (can_refract && shlick_reflect(theta_cos, refractive_ratio) < random_double()) {
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

    static double shlick_reflect(double cos, double refractive_index) {
        double r0 = (1 - refractive_index) / (1 + refractive_index);
        double r0_squared = r0 * r0;
        return r0_squared + (1-r0_squared) * pow((1-cos), 5); // r^2 + (1-r^2) * (1-cos)^5
    }
};
#endif //GRAPHICA_MATERIAL_H
