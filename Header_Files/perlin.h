//
// Created by Aryan Singh on 6/9/24.
//

#ifndef GRAPHICA_PERLIN_H
#define GRAPHICA_PERLIN_H

#include "constants.h"

class perlin {
public:
    perlin() {
        randvec = new vec3[num_of_points];
        for (int i = 0; i < num_of_points; i++) {
            randvec[i] = unit_vector(vec3::random_vector());
        }
        perm_x = generate_permutation();
        perm_y = generate_permutation();
        perm_z = generate_permutation();
    }

    ~perlin() {
        delete[] randvec;
        delete[] perm_x;
        delete[] perm_y;
        delete[] perm_z;
    }

    double noise(const point3& p) const {

        auto u = p.x() - floor(p.x());
        auto v = p.y() - floor(p.y());
        auto w = p.z() - floor(p.z());

        auto i = int(floor(p.x()));
        auto j = int(floor(p.y()));
        auto k = int(floor(p.z()));
        vec3 c[2][2][2];

        for (int di = 0; di < 2; di++) {
            for (int dj = 0; dj < 2; dj++) {
                for (int dk = 0; dk < 2; dk++) {
                    c[di][dj][dk] = randvec[
                            perm_x[(di+i) & 255]
                            ^ perm_y[(dj+j) & 255]
                            ^ perm_z[(dk+k) & 255]
                            ]; // can also do mod 256 but bitwise is faster
                }
            }
        }

        return trilinear_interpolate(c, u, v, w); // for smoothness
    }

    double turbulence(const point3& p, int iterations) const {
        auto sum = 0.0;
        auto tmp = p;
        auto weight = 1.0;
        for (int i = 0; i < iterations; i++) {
            sum += weight * noise(tmp);
            weight *= 0.5;
            tmp *= 2;
        }
        return fabs(sum);
    }

private:
    static const int num_of_points = 256;
//    double* randfloat;
    vec3* randvec;
    int* perm_x;
    int* perm_y;
    int* perm_z;

    static int* generate_permutation() {
        auto p = new int[num_of_points];
        for (int i = 0; i < num_of_points; i++) {
            p[i] = i;
        }
        permute(p, num_of_points);
        return p;
    }

    static void permute(int* p, int n) {
//        clog << "Reached here \n";
        for (int i = n-1; i > 0; i--) {
            int target = random_int(0, i);
            int tmp = p[i];
            p[i] = p[target];
            p[target] = tmp;
        }
//        clog << "Reached here 2 \n";
    }

    static double trilinear_interpolate(vec3 c[2][2][2], double u, double v, double w) {

        auto new_u = u*u*(3-2*u);
        auto new_v = v*v*(3-2*v);
        auto new_w = w*w*(3-2*w);
        auto sum = 0.0;
        for (int i = 0; i < 2; i++) {
            for (int j = 0; j < 2; j++) {
                for (int k = 0; k < 2; k++) {
                    vec3 weight_vector(u-i, v-j, w-k);
                    sum += dot(c[i][j][k], weight_vector) *
                            (i * new_u + (1-i) * (1-new_u)) *
                            (j * new_v + (1-j) * (1-new_v)) *
                            (k * new_w + (1-k) * (1-new_w));
                }
            }
        }

        return sum;
    }
};

#endif //GRAPHICA_PERLIN_H
