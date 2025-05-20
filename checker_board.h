#ifndef CHECKERBOARDH
#define CHECKERBOARDH

#include "point.h"
#include "draw.h"

const int inf = 50;

class CheckerBoard {
    

    public:
    double width;
    double ambient;
    double diffuse;
    double reflection;

    CheckerBoard() = default;
    CheckerBoard(double w, double a, double d, double r)
        : width(w), ambient(a), diffuse(d), reflection(r) {}

    void draw() {
        int white=0, black;

        for(int i=-inf; i<inf; i++){
            black=white;
            for(int j=-inf; j<inf; j++){
                // point tempPoint(i*width,j*width,0);
                drawSquare(point(i*width, j*width, 0), width, black);
                black=1-black;
            }
            white=1-white;
        }
    }
};

#endif
