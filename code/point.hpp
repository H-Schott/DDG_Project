#ifndef __POINT_HPP__
#define __POINT_HPP__

#include <iostream>

class Point3 {
public:
    double x;
    double y;
    double z;
public:
    Point3() { x = 0; y = 0; z = 0; };
    explicit Point3(double _x, double _y, double _z) { x = _x; y = _y; z = _z; };
    explicit Point3(const Point3& pt) : x(double(pt.x)), y(double(pt.y)), z(double(pt.z)) {};

    double operator[](uint id) const { if(id == 0) return x; if(id == 1) return y; return z; };
    double& operator[](uint id) { if(id == 0) return x; if(id == 1) return y; return z; };
    
    Point3 operator+(const Vector3& vec) const { return Point3(x + vec.x, y + vec.y, z + vec.z); };
    Point3 operator+(const Point3& pt) const { return Point3(x + pt.x, y + pt.y, z + pt.z); };
    Point3 operator-(const Vector3& vec) const { return (*this) + (-vec); };
    Vector3 operator-(const Point3& pt) const { return Vector3(x - pt.x, y - pt.y, z - pt.z); };
    Point3 operator*(double a) const { return Point3(a * x, a * y, a * z); };
    Point3 operator/(double a) const { return Point3(x / a, y / a, z / a); };
};


// Point3 Inline Functions -----------------------------------------------------------

inline std::ostream& operator<<(std::ostream& os, const Point3 pt) {
    os << "(" << pt.x << ", " << pt.y << ", " << pt.z << ")";
    return os;
}

inline double Distance(const Point3& p1, const Point3& p2) {
    return (p1 - p2).Norm();
}

inline double SquaredDistance(const Point3& p1, const Point3& p2) {
    return (p1 - p2).SquaredNorm();
}


inline Point3 operator*(double a, const Point3& pt) { return pt * a; }

Point3 Lerp(double t, const Point3& p0, const Point3& p1) { return (1 - t) * p0 + t * p1; }

Point3 Max(const Point3& p1, const Point3& p2) {
    return Point3(std::max(p1.x, p2.x), std::max(p1.y, p2.y), std::max(p1.z, p2.z));
}

Point3 Min(const Point3& p1, const Point3& p2) {
    return Point3(std::min(p1.x, p2.x), std::min(p1.y, p2.y), std::min(p1.z, p2.z));
}

Point3 Floor(const Point3& pt) {
    return Point3(std::floor(pt.x), std::floor(pt.y), std::floor(pt.z));
}

Point3 Ceil(const Point3& pt) {
    return Point3(std::ceil(pt.x), std::ceil(pt.y), std::ceil(pt.z));
}

Point3 Abs(const Point3& pt) {
    return Point3(std::abs(pt.x), std::abs(pt.y), std::abs(pt.z));
}

Point3 Permute(const Point3& pt, uint x, uint y, uint z) {
    return Point3(pt[x], pt[y], pt[z]);
}


#endif