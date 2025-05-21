#ifndef OBJECTH
#define OBJECTH

#include <iostream>
#include "point.h"
#include "color.h"
#include "GL/glut.h"
#include <math.h>
#include <utility>


typedef std::pair<double, point> pdp;

class Object {
public:
    point center;
    Color c;
    double ambient, diffuse, specular, reflection;
    double shininess;

    Object(point p, Color cl, double a, double d, double s, double r, double shini)
        : center(p), c(cl), ambient(a), diffuse(d), specular(s), reflection(r), shininess(shini) {}

    virtual void draw() = 0;
    virtual pdp getIntersection(point p,point vec) = 0;
    virtual ~Object() = default;
};

class Sphere : public Object {
public:
    double radius;

    Sphere(point p, Color cl, double a, double d, double s, double r, double shini, double rad)
        : Object(p, cl, a, d, s, r, shini), radius(rad) {}

    virtual void draw();
    virtual pdp getIntersection(point p,point vec);
};

class Pyramid : public Object {
public:
    double width, height;

    Pyramid(point p, Color cl, double a, double d, double s, double r, double shini, double wid, double hi)
        : Object(p, cl, a, d, s, r, shini), width(wid), height(hi) {}

    virtual void draw();
    virtual pdp getIntersection(point p,point vec);
};

#endif
