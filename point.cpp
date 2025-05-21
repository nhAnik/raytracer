#include "point.h"

point::point(double a, double b, double c) : x(a), y(b), z(c) {}

void point::printPoint() const {
    printf("%.2lf,%.2lf,%.2lf\n", x, y, z);
}

point point::operator+(const point& p) const {
    return point(x + p.x, y + p.y, z + p.z);
}

point point::operator+(double m) const {
    return point(x + m, y + m, z + m);
}

point point::operator-(const point& p) const {
    return point(x - p.x, y - p.y, z - p.z);
}

point point::operator*(double m) const {
    return point(x * m, y * m, z * m);
}

void point::normalize() {
    double magn = sqrt(x * x + y * y + z * z);
    if (magn != 0) {
        x /= magn; y /= magn; z /= magn;
    }
}

point point::get_norm() const {
    double magn = sqrt(x * x + y * y + z * z);
    if (magn == 0) return point(0, 0, 0);
    return point(x / magn, y / magn, z / magn);
}

// Utility function definitions
point getCrossProd(point a, point b) {
    return point(
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x
    );
}

double getDotProd(point p1, point p2) {
    return p1.x * p2.x + p1.y * p2.y + p1.z * p2.z;
}

point getVec(point p1, point p2) {
    return point(p2.x - p1.x, p2.y - p1.y, p2.z - p1.z);
}

point getUnitVec(point p1, point p2) {
    double a = p2.x - p1.x;
    double b = p2.y - p1.y;
    double c = p2.z - p1.z;
    double mag = sqrt(a * a + b * b + c * c);
    if (mag == 0) return point(0, 0, 0);
    return point(a / mag, b / mag, c / mag);
}

point getNormalOnPlane(point a, point b, point c) {
    point p1 = getVec(c, b);
    point p2 = getVec(c, a);
    point p = getCrossProd(p1, p2);
    p.normalize();
    return p;
}

double getDeterminants(double arr[3][3]) {
    double det1 = arr[0][0] * (arr[1][1] * arr[2][2] - arr[1][2] * arr[2][1]);
    double det2 = arr[0][1] * (arr[1][0] * arr[2][2] - arr[2][0] * arr[1][2]);
    double det3 = arr[0][2] * (arr[1][0] * arr[2][1] - arr[1][1] * arr[2][0]);
    return det1 - det2 + det3;
}

double getTriangleT(point a, point b, point c, point p, point vec) {
    double beta, gamma, t = -1, det;

    double A[3][3] = {
        {a.x - b.x, a.x - c.x, vec.x},
        {a.y - b.y, a.y - c.y, vec.y},
        {a.z - b.z, a.z - c.z, vec.z}
    };

    double A1[3][3] = {
        {a.x - p.x, a.x - c.x, vec.x},
        {a.y - p.y, a.y - c.y, vec.y},
        {a.z - p.z, a.z - c.z, vec.z}
    };

    double A2[3][3] = {
        {a.x - b.x, a.x - p.x, vec.x},
        {a.y - b.y, a.y - p.y, vec.y},
        {a.z - b.z, a.z - p.z, vec.z}
    };

    double A3[3][3] = {
        {a.x - b.x, a.x - c.x, a.x - p.x},
        {a.y - b.y, a.y - c.y, a.y - p.y},
        {a.z - b.z, a.z - c.z, a.z - p.z}
    };

    det = getDeterminants(A);
    if (det != 0) {
        beta = getDeterminants(A1) / det;
        gamma = getDeterminants(A2) / det;
        t = getDeterminants(A3) / det;

        if (beta + gamma < 1 && beta > 0 && gamma > 0 && t > 0)
            return t;
    }

    return -1;
}

double getDistOfPoints(point p1, point p2) {
    return sqrt(
        (p1.x - p2.x) * (p1.x - p2.x) +
        (p1.y - p2.y) * (p1.y - p2.y) +
        (p1.z - p2.z) * (p1.z - p2.z)
    );
}

point rotateVec(point a, point p) {
    double cameraAngle = 0.03;
    a.x = a.x * cos(cameraAngle) + p.x * sin(cameraAngle);
    a.y = a.y * cos(cameraAngle) + p.y * sin(cameraAngle);
    a.z = a.z * cos(cameraAngle) + p.z * sin(cameraAngle);
    a.normalize();
    return a;
}

point rotateVecAng(point a, point p, double angle) {
    a.x = a.x * cos(angle) + p.x * sin(angle);
    a.y = a.y * cos(angle) + p.y * sin(angle);
    a.z = a.z * cos(angle) + p.z * sin(angle);
    return a;
}
