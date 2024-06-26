//
// Created by Aryan Singh on 1/9/24.
//

#ifndef GRAPHICA_ENTITY_LIST_H
#define GRAPHICA_ENTITY_LIST_H
#include <utility>
#include <vector>
#include <memory>
#include "entity.h"
#include "axis_aligned_bounding_box.h"

using namespace std;
class entity_list : public entity {
public:
    vector<shared_ptr<entity>> objects;

    entity_list() = default;
    explicit entity_list(shared_ptr<entity> object) { add(std::move(object));}

    void clear() { objects.clear(); }

    void add(const shared_ptr<entity>& object) {
        objects.push_back(object);
        bbox = axis_aligned_bounding_box(bbox, object->bounding_box());
    }

    [[nodiscard]] axis_aligned_bounding_box bounding_box() const override {
        return bbox;
    }

    bool hit(const ray& r, interval ray_t, entity_record& record) const override {
         entity_record temp_record;
         bool hit_anything = false;
         auto closest_so_far = ray_t.max;

         for (const auto& object : objects) {
             if (object->hit(r, interval(ray_t.min, closest_so_far), temp_record)) {
                 hit_anything = true;
                 closest_so_far = temp_record.t;
                 record = temp_record;
             }
         }

         return hit_anything;
    }

private:
    axis_aligned_bounding_box bbox;
};

#endif //GRAPHICA_ENTITY_LIST_H
