#include "object.h"

pdp Pyramid::getIntersection(point p,point vec) {
    double tempT[6],minT=100000;
    int minIdx;
    point normal(0,0,0);

    point topPoint(center.x+width/2, center.y+width/2, center.z+height);
    point upperLeft(center.x,        center.y+width,   center.z);
    point lowerRight(center.x+width, center.y ,        center.z);
    point upperRight(center.x+width, center.y+width,   center.z);

    tempT[0] = getTriangleT(center,  upperLeft,  topPoint,   p,vec);
    tempT[1] = getTriangleT(upperLeft,  upperRight, topPoint,   p,vec);
    tempT[2] = getTriangleT(upperRight, lowerRight, topPoint,   p,vec);
    tempT[3] = getTriangleT(lowerRight, center,  topPoint,   p,vec);
    tempT[4] = getTriangleT(center,  upperLeft,  upperRight, p,vec);
    tempT[5] = getTriangleT(center,  upperRight, lowerRight, p,vec);
    //tempT[4]=getRectangleT(center,width,p,vec);

    if (tempT[0] ==-1 && tempT[1]==-1 && tempT[2] ==-1 &&
        tempT[3] ==-1 && tempT[4] ==-1 && tempT[5]==-1) {
        minT=-1;
        return pdp(minT,normal);
    } else {
        for (int i = 0; i < 6; i++) {
            if (tempT[i] != -1 && tempT[i] < minT) {
                minT = tempT[i];
                minIdx = i;
            }
        }
    }

    if (minIdx == 0) normal = getNormalOnPlane(upperLeft,center,topPoint);
    else if (minIdx == 1) normal = getNormalOnPlane(upperRight,upperLeft,topPoint);
    else if (minIdx == 2) normal = getNormalOnPlane(lowerRight,upperRight,topPoint);
    else if (minIdx == 3) normal = getNormalOnPlane(center,lowerRight,topPoint);
    else if (minIdx == 4 || minIdx == 5) normal.z=-1;

    return pdp(minT,normal);
}

void Pyramid::draw() {
    point topPoint(center.x+width/2, center.y+width/2, center.z+height);
    glColor3f(c.r,c.g,c.b);
    glBegin(GL_QUADS);
    {
        glVertex3f(center.x,         center.y,       center.z);
        glVertex3f(center.x + width, center.y,       center.z);
        glVertex3f(center.x + width, center.y+width, center.z);
        glVertex3f(center.x,         center.y+width, center.z);
    }
    glEnd();

    glBegin(GL_TRIANGLES);
    {
        glVertex3f(topPoint.x,       topPoint.y, topPoint.z);
        glVertex3f(center.x,         center.y,   center.z);
        glVertex3f(center.x + width, center.y,   center.z);
    }
    glEnd();

    glBegin(GL_TRIANGLES);
    {
        glVertex3f(topPoint.x,topPoint.y,topPoint.z);
        glVertex3f(center.x+width, center.y, center.z);
        glVertex3f(center.x+width, center.y+width, center.z);
    }
    glEnd();

    glBegin(GL_TRIANGLES);
    {
        glVertex3f(topPoint.x,topPoint.y,topPoint.z);
        glVertex3f(center.x+width, center.y+width, center.z);
        glVertex3f(center.x, center.y+width, center.z);
    }
    glEnd();

    glBegin(GL_TRIANGLES);
    {
        glVertex3f(topPoint.x,topPoint.y,topPoint.z);
        glVertex3f(center.x, center.y+width, center.z);
        glVertex3f(center.x, center.y, center.z);
    }
    glEnd();
}
