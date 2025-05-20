#ifndef POINTH
#define POINTH

#include<math.h>
#include<stdio.h>


class point{
public:
	double x, y, z;

	point(double a, double b, double c){
        x=a; y=b; z=c;
	}

	point(){}

	void printPoint(){
        printf("%.2lf,%.2lf,%.2lf\n",x,y,z);
	}

	point operator+(const point& p) {
	    return point(x + p.x, y + p.y, z + p.z);
	}

	point operator+(double m){
	    point p1(x+m,y+m,z+m);
	    return p1;
	}

	point operator-(const point& p){
	    point p1(x-p.x,y-p.y,z-p.z);
	    return p1;
	}

	point operator*(double m){
        point p1(m*x,m*y,m*z);
        return p1;
	}

	void normalize(){
        double magn=sqrt(x*x+y*y+z*z);
        x=x/magn; y=y/magn; z=z/magn;
	}

    point get_norm() {
        double magn=sqrt(x*x+y*y+z*z);
        x=x/magn; y=y/magn; z=z/magn;
        return point(x, y, z);
    }
};

point getCrossProd(point a, point b){

    point temp;
    temp.x=a.y*b.z-a.z*b.y;
    temp.y=a.z*b.x-a.x*b.z;
    temp.z=a.x*b.y-a.y*b.x;
    return temp;
}

double getDotProd(point p1,point p2){
    double d=p1.x*p2.x+p1.y*p2.y+p1.z*p2.z;
    return d;
}

point getVec(point p1, point p2){
    point v(p2.x-p1.x,p2.y-p1.y,p2.z-p1.z);
    return v;
}

point getUnitVec(point p1, point p2){
    double a=p2.x-p1.x;
    double b=p2.y-p1.y;
    double c=p2.z-p1.z;
    double mag=sqrt(a*a+b*b+c*c);
    a=a/mag;
    b=b/mag;
    c=c/mag;
    point v(a,b,c);
    return v;
}

point getNormalOnPlane(point a,point b,point c){
    point p1=getVec(c,b);
    point p2=getVec(c,a);
    point p=getCrossProd(p1,p2);
    p.normalize();
    return p;
}

double getDeterminants(double arr[3][3]){
    double det1=arr[0][0]*(arr[1][1]*arr[2][2]-arr[1][2]*arr[2][1]);
    double det2=arr[0][1]*(arr[1][0]*arr[2][2]-arr[2][0]*arr[1][2]);
    double det3=arr[0][2]*(arr[1][0]*arr[2][1]-arr[1][1]*arr[2][0]);
    return det1-det2+det3;
}

double getTriangleT(point a,point b,point c,point p,point vec){
    double beta, gamma, t=-1, det;
    double A[3][3] ={{a.x-b.x, a.x-c.x, vec.x},  {a.y-b.y, a.y-c.y, vec.y},  {a.z-b.z, a.z-c.z, vec.z}};
    double A1[3][3]={{a.x-p.x, a.x-c.x, vec.x},  {a.y-p.y, a.y-c.y, vec.y},  {a.z-p.z, a.z-c.z, vec.z}};
    double A2[3][3]={{a.x-b.x, a.x-p.x, vec.x},  {a.y-b.y, a.y-p.y, vec.y},  {a.z-b.z, a.z-p.z, vec.z}};
    double A3[3][3]={{a.x-b.x, a.x-c.x, a.x-p.x},{a.y-b.y, a.y-c.y, a.y-p.y},{a.z-b.z, a.z-c.z, a.z-p.z}};

    det=getDeterminants(A);
    if (det!=0){
        beta=getDeterminants(A1)/det;
        gamma=getDeterminants(A2)/det;
        t=getDeterminants(A3)/det;

        if(beta+gamma<1 && beta>0 && gamma>0 && t>0) return t;
        else return -1;
    }


    return t;

}

double getDistOfPoints(point p1,point p2){
    double d=sqrt((p1.x-p2.x)*(p1.x-p2.x)+(p1.y-p2.y)*(p1.y-p2.y)+(p1.z-p2.z)*(p1.z-p2.z));
    return d;
}


point rotateVec(point a, point p){
    double cameraAngle = 0.03;
    a.x=a.x*cos(cameraAngle)+p.x*sin(cameraAngle);
    a.y=a.y*cos(cameraAngle)+p.y*sin(cameraAngle);
    a.z=a.z*cos(cameraAngle)+p.z*sin(cameraAngle);
    a.normalize();
    return a;
}

point rotateVecAng(point a, point p, double angle){
    a.x=a.x*cos(angle)+p.x*sin(angle);
    a.y=a.y*cos(angle)+p.y*sin(angle);
    a.z=a.z*cos(angle)+p.z*sin(angle);
    return a;
}

#endif