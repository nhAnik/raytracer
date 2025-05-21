#ifndef POINTH
#define POINTH

#include <cmath>
#include <cstdio>

class point {
public:
    double x, y, z;

    point(double a = 0, double b = 0, double c = 0);
    void printPoint() const;

    point operator+(const point& p) const;
    point operator+(double m) const;
    point operator-(const point& p) const;
    point operator*(double m) const;

    void normalize();
    point get_norm() const;
};

// Function declarations
point getCrossProd(point a, point b);
double getDotProd(point p1, point p2);
point getVec(point p1, point p2);
point getUnitVec(point p1, point p2);
point getNormalOnPlane(point a, point b, point c);
double getDeterminants(double arr[3][3]);
double getTriangleT(point a, point b, point c, point p, point vec);
double getDistOfPoints(point p1, point p2);
point rotateVec(point a, point p);
point rotateVecAng(point a, point p, double angle);

#endif // POINTH
