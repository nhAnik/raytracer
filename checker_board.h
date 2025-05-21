#ifndef CHECKERBOARDH
#define CHECKERBOARDH

#include "point.h"
#include "GL/glut.h"

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

        for(int i=-inf; i<inf; i++) {
            black=white;
            for(int j=-inf; j<inf; j++) {
                drawSquare(point(i*width, j*width, 0), width, black);
                black=1-black;
            }
            white=1-white;
        }
    }

    inline void drawSquare(point p, double width, int color) {
        if (color==0) glColor3f(0.0,0.0,0.0);
        else if (color==1) glColor3f(1.0,1.0,1.0);
        glBegin(GL_QUADS);
        {
            glVertex3f(p.x, p.y, p.z);
            glVertex3f(p.x+width, p.y, p.z);
            glVertex3f(p.x+width, p.y+width, p.z);
            glVertex3f(p.x, p.y+width, p.z);

        }
        glEnd();
    }
};

#endif
