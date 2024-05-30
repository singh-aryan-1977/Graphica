//
// Created by Aryan Singh on 1/7/24.
//

#include "Header_Files/color.h"
#include "Header_Files/vec3.h"
#include "Header_Files/constants.h"
#include "Header_Files/sphere.h"
#include "Header_Files/entity.h"
#include "Header_Files/entity_list.h"
#include "Header_Files/ray.h"
#include "Header_Files/camera.h"
#include <iostream>

using namespace std;

int main() {
    // World building
    entity_list world;
    world.add(make_shared<sphere>(point3(0,0,-1),0.5));
    world.add(make_shared<sphere>(point3(0,-100.5,-1),100));

    camera cam;
    cam.aspect_ratio = 16.0 / 9.0;
    cam.IMAGE_WIDTH = 400;

    cam.render(world);
    return 0;
}
