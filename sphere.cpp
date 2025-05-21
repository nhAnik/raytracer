#include "object.h"

#define pi (2*acos(0.0))

pdp Sphere::getIntersection(point p, point vec) {
    double a, b, c, t1, t2, t = -1;
    point normal(0,0,0);
    point temp = p + center*(-1);
    a = 1;
    b = 2 * (vec.x*temp.x + vec.y*temp.y + vec.z*temp.z);
    c = (temp.x*temp.x + temp.y*temp.y + temp.z*temp.z) - radius*radius;
    double discriminant = b*b - 4*a*c;
    if (discriminant >= 0) {
        t1 = (-b+sqrt(discriminant)) / (2.0*a);
        t2 = (-b-sqrt(discriminant)) / (2.0*a);


        if (t1>0 && t2>0) {
            t = std::min(t1,t2);
        } else {
            if (t1 > 0) t = t1;
            else if (t2 > 0) t = t2;
            else t = -1;
        }
    }
    if (t != -1) {
        normal = getUnitVec(center, p+vec*t);
    }

    return pdp(t,normal);
}

void Sphere::draw() {
    int slices = 24, stacks = 20;
    point points[100][100];
    int i, j;
    double h, r;

    for(i = 0; i <= stacks; i++) {
        h = radius * sin(((double)i/(double)stacks)*(pi/2));
        r = radius * cos(((double)i/(double)stacks)*(pi/2));

        for(j = 0; j <= slices; j++) {
            points[i][j].x = center.x + r*cos(((double)j/(double)slices)*2*pi);
            points[i][j].y = center.y + r*sin(((double)j/(double)slices)*2*pi);
            points[i][j].z = h;
        }
    }
    for(i = 0; i <= stacks; i++) {
        glColor3f(c.r, c.g, c.b);
        for (j = 0; j < slices; j++) {
            glBegin(GL_QUADS);
            {
                glVertex3f(points[i][j].x,     points[i][j].y,     center.z+points[i][j].z);
                glVertex3f(points[i][j+1].x,   points[i][j+1].y,   center.z+points[i][j+1].z);
                glVertex3f(points[i+1][j+1].x, points[i+1][j+1].y, center.z+points[i+1][j+1].z);
                glVertex3f(points[i+1][j].x,   points[i+1][j].y,   center.z+points[i+1][j].z);

                glVertex3f(points[i][j].x,     points[i][j].y,     center.z-points[i][j].z);
                glVertex3f(points[i][j+1].x,   points[i][j+1].y,   center.z-points[i][j+1].z);
                glVertex3f(points[i+1][j+1].x, points[i+1][j+1].y, center.z-points[i+1][j+1].z);
                glVertex3f(points[i+1][j].x,   points[i+1][j].y,   center.z-points[i+1][j].z);
            }
            glEnd();
        }
    }
}
