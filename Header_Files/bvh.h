//
// Created by Aryan Singh on 6/8/24.
//

#ifndef GRAPHICA_BVH_H
#define GRAPHICA_BVH_H

#include "constants.h"
#include "axis_aligned_bounding_box.h"
#include "entity_list.h"
#include "entity.h"
#include <algorithm>

using namespace std;

class bvh : public entity {
public:
    explicit bvh(entity_list list): bvh(list.objects, 0, list.objects.size()) {}

    bvh(vector<shared_ptr<entity>>& objects, size_t start, size_t end) {
        bbox = axis_aligned_bounding_box::empty;
        for (size_t i = start; i < end; i++) {
            bbox = axis_aligned_bounding_box(bbox, objects[i]->bounding_box());
        }
        int axis = bbox.longest_axis();

        auto cmp = (axis == 0) ? box_x_compare : (axis == 1) ? box_y_compare : box_z_compare;

        size_t len = end-start;
        if (len == 1) {
            left = right = objects[start];
        } else if (len == 2) {
            left = objects[start];
            right = objects[start + 1];
        } else {
            sort(objects.begin()+start, objects.begin()+end, cmp);
            auto mid = start + len/2;

            left = make_shared<bvh>(objects, start, mid);
            right = make_shared<bvh>(objects, mid, end);
        }
//        bbox = axis_aligned_bounding_box(left->bounding_box(), right->bounding_box());
    }

    bool hit(const ray& incidence, interval ray, entity_record& record) const override {
        if (!bbox.hit(incidence, ray)) {
            return false;
        }
        bool l = left->hit(incidence, ray, record);
        // need to update interval if l was found to find closest intersection point (shrink it)
        bool r = right->hit(incidence, interval(ray.min, l ? record.t : ray.max), record);
        return l || r;
    }

    axis_aligned_bounding_box bounding_box() const override { return bbox; }

private:
    shared_ptr<entity> left, right;
    axis_aligned_bounding_box bbox;

    static bool box_compare(const shared_ptr<entity>& a, const shared_ptr<entity>& b, int axis) {
        auto a_axis = a->bounding_box().axis_of_interval(axis);
        auto b_axis = b->bounding_box().axis_of_interval(axis);
        return a_axis.min < b_axis.min;
    }

    static bool box_x_compare(const shared_ptr<entity>& a, const shared_ptr<entity>& b) {
        return box_compare(a,b,0);
    }

    static bool box_y_compare(const shared_ptr<entity>& a, const shared_ptr<entity>& b) {
        return box_compare(a,b,1);
    }

    static bool box_z_compare(const shared_ptr<entity>& a, const shared_ptr<entity>& b) {
        return box_compare(a,b,2);
    }
};

#endif //GRAPHICA_BVH_H
