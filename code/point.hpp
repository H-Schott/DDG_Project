#ifndef __POINT_HPP__
#define __POINT_HPP__

#include <iostream>

#define uint unsigned int

class Vector;

class Point {
public:
    double x;
    double y;
    double z;
public:
    Point();
    Point(double _x, double _y, double _z);
    Point(const Point& pt);

    double operator[](uint id) const;
    double& operator[](uint id);
    
    Point operator+(const Vector& vec) const;
    Point operator+(const Point& pt) const;
    Point operator-(const Vector& vec) const;
    Vector operator-(const Point& pt) const;
    Point operator*(double a) const;
    Point operator/(double a) const;
};


// Point Inline Functions -----------------------------------------------------------

inline std::ostream& operator<<(std::ostream& os, const Point pt) {
    os << "(" << pt.x << ", " << pt.y << ", " << pt.z << ")";
    return os;
}

inline double Distance(const Point& p1, const Point& p2) {
    return (p1 - p2).Norm();
}

inline double SquaredDistance(const Point& p1, const Point& p2) {
    return (p1 - p2).SquaredNorm();
}


inline Point operator*(double a, const Point& pt) { return pt * a; }

Point Lerp(double t, const Point& p0, const Point& p1) { return (1 - t) * p0 + t * p1; }

Point Max(const Point& p1, const Point& p2) {
    return Point(std::max(p1.x, p2.x), std::max(p1.y, p2.y), std::max(p1.z, p2.z));
}

Point Min(const Point& p1, const Point& p2) {
    return Point(std::min(p1.x, p2.x), std::min(p1.y, p2.y), std::min(p1.z, p2.z));
}

Point Floor(const Point& pt) {
    return Point(std::floor(pt.x), std::floor(pt.y), std::floor(pt.z));
}

Point Ceil(const Point& pt) {
    return Point(std::ceil(pt.x), std::ceil(pt.y), std::ceil(pt.z));
}

Point Abs(const Point& pt) {
    return Point(std::abs(pt.x), std::abs(pt.y), std::abs(pt.z));
}

Point Permute(const Point& pt, uint x, uint y, uint z) {
    return Point(pt[x], pt[y], pt[z]);
}


#endif