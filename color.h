#ifndef COLORH
#define COLORH

#include<stdio.h>

class Color {
public:
    double r, g, b;

    Color() = default;

    Color(double r, double g, double b) : r(r), g(g), b(b) {}

    Color operator+(const Color& c1) {
        return Color(r + c1.r, g + c1.g, b + c1.b);
    }

    Color operator*(double multiplier) {
        return Color(multiplier*r, multiplier*g, multiplier*b);
    }
};

#endif
