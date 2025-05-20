#ifndef COLORH
#define COLORH

#include<stdio.h>

class Color {
public:
    double r, g, b;
    Color(double r, double g, double b) {
        this->r = r;
        this->g = g;
        this->b = b;
    }
    Color() {
    }

    Color operator+(const Color& c1){
        Color c(r+c1.r,g+c1.g,b+c1.b);
        return c;
    }

    Color operator*(double m){
        Color c1(m*r,m*g,m*b);
        return c1;
    }

    void printColor(){
        printf("%.3lf,%.3lf,%.3lf\n",r,g,b);
	}
};

#endif