//
// Created by Aryan Singh on 6/10/24.
//

#ifndef GRAPHICA_PDF_H
#define GRAPHICA_PDF_H

#include "onb.h"
#include "constants.h"
#include "entity_list.h"

class pdf {
public:
    virtual ~pdf() {}

    virtual double value(const vec3& direction) const = 0;
    virtual vec3 generate() const=0;
};

class sphere_pdf : public pdf {
public:
    sphere_pdf() {}

    double value(const vec3& direction) const override {
        return 1 / (4*pi);
    }

    vec3 generate() const override {
        return normalize_vec_in_unit_sphere();
    }
};

class cosine_pdf : public pdf {
public:
    cosine_pdf(const vec3& basis) {coord_system.build(basis);}

    double value(const vec3& direction) const override {
        auto cos_theta = dot(unit_vector(direction), coord_system.w());
        return fmax(0, cos_theta/pi);
    }

    vec3 generate() const override {
        return coord_system.local(random_cos_direction());
    }
private:
    onb coord_system;
};


class entity_pdf : public pdf {
public:
    entity_pdf(const entity& objects, point3& origin) : objects(objects), origin(origin) {}

    double value(const vec3& direction) const override {
        return objects.pdf_value(origin, direction);
    }

    vec3 generate() const override {
        return objects.random(origin);
    }

private:
    const entity& objects;
    point3 origin;
};

class mixture_pdf : public pdf {
public:
    mixture_pdf(shared_ptr<pdf> alpha_pdf, shared_ptr<pdf> beta_pdf) {
        pdfs[0] = alpha_pdf;
        pdfs[1] = beta_pdf;
    }

    double value(const vec3& direction) const override {
        return 0.5 * pdfs[0]->value(direction) + 0.5*pdfs[1]->value(direction);
    }

    vec3 generate() const override {
        if (random_double() < 0.5) {
            return pdfs[0]->generate();
        } else {
            return pdfs[1]->generate();
        }
    }

private:
    shared_ptr<pdf> pdfs[2];
};
#endif //GRAPHICA_PDF_H
