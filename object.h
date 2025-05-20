#ifndef OBJECTH
#define OBJECTH

#include <iostream>
#include "point.h"
#include "color.h"
#include "GL/glut.h"
#include <math.h>
#include <utility>

#define pi (2*acos(0.0))

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
    virtual pdp getT(point p,point vec) = 0;
    virtual ~Object() = default;
};

class Sphere : public Object {
public:
    double radius;

    Sphere(point p, Color cl, double a, double d, double s, double r, double shini, double rad)
        : Object(p, cl, a, d, s, r, shini), radius(rad) {}

    pdp getT(point p, point vec){
        double a,b,c,t1,t2,t=-1;
        point normal(0,0,0);
        point temp=p+center*(-1);
        a=1;
        b=2*(vec.x*temp.x+vec.y*temp.y+vec.z*temp.z);
        c=(temp.x*temp.x+temp.y*temp.y+temp.z*temp.z)-radius*radius;
        double discrim=b*b-4*a*c;
        if (discrim>=0){
            t1=(-b+sqrt(discrim))/(2.0*a);
            t2=(-b-sqrt(discrim))/(2.0*a);


            if(t1>0 && t2>0) t=std::min(t1,t2);
            else{
                if(t1>0) t=t1;
                else if(t2>0) t=t2;
                else t=-1;
            }
        }
        if (t!=-1) normal=getUnitVec(center,p+vec*t);

        return pdp(t,normal);
    }

    void draw() {
        int slices=24,stacks=20;
        struct point points[100][100];
        int i,j;
        double h,r;

        for(i=0;i<=stacks;i++)
        {
            h=radius*sin(((double)i/(double)stacks)*(pi/2));
            r=radius*cos(((double)i/(double)stacks)*(pi/2));
            for(j=0;j<=slices;j++)
            {
                points[i][j].x=center.x + r*cos(((double)j/(double)slices)*2*pi);
                points[i][j].y=center.y + r*sin(((double)j/(double)slices)*2*pi);
                points[i][j].z=h;
            }
        }
        for(i=0;i<stacks;i++)
        {
            glColor3f(c.r,c.g,c.b);
            for(j=0;j<slices;j++)
            {
                glBegin(GL_QUADS);{

                    glVertex3f(points[i][j].x,     points[i][j].y,     center.z+points[i][j].z);
                    glVertex3f(points[i][j+1].x,   points[i][j+1].y,   center.z+points[i][j+1].z);
                    glVertex3f(points[i+1][j+1].x, points[i+1][j+1].y, center.z+points[i+1][j+1].z);
                    glVertex3f(points[i+1][j].x,   points[i+1][j].y,   center.z+points[i+1][j].z);

                    glVertex3f(points[i][j].x,     points[i][j].y,     center.z-points[i][j].z);
                    glVertex3f(points[i][j+1].x,   points[i][j+1].y,   center.z-points[i][j+1].z);
                    glVertex3f(points[i+1][j+1].x, points[i+1][j+1].y, center.z-points[i+1][j+1].z);
                    glVertex3f(points[i+1][j].x,   points[i+1][j].y,   center.z-points[i+1][j].z);
                }glEnd();
            }
        }
    }
};

class Pyramid : public Object {
public:
    double width, height;

    Pyramid(point p, Color cl, double a, double d, double s, double r, double shini, double wid, double hi)
        : Object(p, cl, a, d, s, r, shini), width(wid), height(hi) {}

    pdp getT(point p,point vec){

        double tempT[6],minT=100000;
        int minIdx;
        point normal(0,0,0);

        point topPoint(center.x+width/2, center.y+width/2, center.z+height);
        point upperLeft(center.x,        center.y+width,   center.z);
        point lowerRight(center.x+width, center.y ,        center.z);
        point upperRight(center.x+width, center.y+width,   center.z);

        tempT[0]=getTriangleT(center,  upperLeft,  topPoint,   p,vec);
        tempT[1]=getTriangleT(upperLeft,  upperRight, topPoint,   p,vec);
        tempT[2]=getTriangleT(upperRight, lowerRight, topPoint,   p,vec);
        tempT[3]=getTriangleT(lowerRight, center,  topPoint,   p,vec);
        tempT[4]=getTriangleT(center,  upperLeft,  upperRight, p,vec);
        tempT[5]=getTriangleT(center,  upperRight, lowerRight, p,vec);
        //tempT[4]=getRectangleT(center,width,p,vec);

        if(tempT[0]==-1 && tempT[1]==-1 && tempT[2]==-1 && tempT[3]==-1 && tempT[4]==-1 && tempT[5]==-1) {
            minT=-1;
            return pdp(minT,normal);
        }
        else {
            for(int i=0;i<6;i++){
                if(tempT[i]!=-1 && tempT[i]<minT){
                    minT=tempT[i];
                    minIdx=i;
                }
            }
        }
        if(minIdx==0) normal=getNormalOnPlane(upperLeft,center,topPoint);
        else if(minIdx==1) normal=getNormalOnPlane(upperRight,upperLeft,topPoint);
        else if(minIdx==2) normal=getNormalOnPlane(lowerRight,upperRight,topPoint);
        else if(minIdx==3) normal=getNormalOnPlane(center,lowerRight,topPoint);
        else if(minIdx==4 || minIdx==5) normal.z=-1;

        return pdp(minT,normal);
    }

    void draw() {
        point topPoint(center.x+width/2, center.y+width/2, center.z+height);
        glColor3f(c.r,c.g,c.b);
        glBegin(GL_QUADS);{
            glVertex3f(center.x,         center.y,       center.z);
            glVertex3f(center.x + width, center.y,       center.z);
            glVertex3f(center.x + width, center.y+width, center.z);
            glVertex3f(center.x,         center.y+width, center.z);
        }glEnd();

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
};

#endif
