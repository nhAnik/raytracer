#include "point.h"
#include "GL/glut.h"


void drawSquare(point p, double width, int color){
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
