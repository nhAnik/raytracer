#ifndef LIGHTH
#define LIGHTH

#include "point.h"

class Light{
public:
    point aPoint;
    double falloff;

    Light(point p, double f) : aPoint(p), falloff(f) {}
};

class SpotLight : public Light{
public: 
    point look;
    double cutoff;

    SpotLight(point p, double f, point lk, double c):
        Light(p,f), look(lk), cutoff(c) {}
};

#endif
