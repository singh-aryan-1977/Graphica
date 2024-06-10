//
// Created by Aryan Singh on 6/8/24.
//

#ifndef GRAPHICA_TEXTURE_H
#define GRAPHICA_TEXTURE_H

#include <utility>
#include "rtw_image.h"
#include "perlin.h"
#include "constants.h"

class texture {
public:
    virtual ~texture() = default;

    virtual color value(double u, double v, const point3& p) const = 0;
};

class solid_color: public texture {
public:
    solid_color(const color& albedo): albedo(albedo) {}
    solid_color(double r, double g, double b): solid_color(color(r,g,b)) {}
    color value(double u, double v, const point3& p) const override {
        return albedo; // since its a solid color, we can just return albedo as we initialized no matter coordinate
    }
private:
    color albedo;
};

class checker: public texture {
public:
    checker(double scale, shared_ptr<texture> even, shared_ptr<texture> odd) :
    inv_scale(1.0/scale), even(std::move(even)), odd(std::move(odd)) {}

    checker(double scale, const color& c1, const color& c2) :
    inv_scale(1.0/scale), even(make_shared<solid_color>(c1)), odd(make_shared<solid_color>(c2)) {}

    [[nodiscard]] color value(double u, double v, const point3& p) const override {
        int x = int(floor(inv_scale * p.x()));
        int y = int(floor(inv_scale * p.y()));
        int z = int(floor(inv_scale * p.z()));

        if ((x+y+z)%2 == 0) {
            return even->value(u,v,p);
        } else {
            return odd->value(u,v,p);
        }
    }

private:
    double inv_scale;
    shared_ptr<texture> even, odd;
};

class image_texture : public texture {
public:
    image_texture(const char* filename) : image(filename) {}

    color value(double u, double v, const point3& p) const override {
        if (image.height() <= 0) {
            return color(0,0,1); // no image so just return solid color
        }

        u = interval(0,1).bound_to(u);
        v = 1.0 - interval(0, 1).bound_to(v);

        auto col = int(u * image.width());
        auto row = int(v * image.height());
        auto pixel_data = image.pixel_data(col, row);

        auto scale = 1.0/255.0;
        return color(pixel_data[0] * scale, pixel_data[1] * scale, pixel_data[2] * scale);


    }
private:
    rtw_image image;
};

class noise_texture: public texture {
public:
    noise_texture() {}

    color value(double u, double v, const point3& p) const override {
        return color(1, 1, 1) * noise.noise(p);
    }
private:
    perlin noise;
};

#endif //GRAPHICA_TEXTURE_H
