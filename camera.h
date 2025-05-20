#ifndef CAMERAH
#define CAMERAH

#include "point.h"

class Camera {
public:
    point pos; // camera position
    point u;   // up vector
    point r;   // right vector
    point l;   // look (forward) vector

    Camera() = default;

    Camera(point pos, point u, point r, point l)
        : pos(pos), u(u), r(r), l(l) {}

};

# endif
