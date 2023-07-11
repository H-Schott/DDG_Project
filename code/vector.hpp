#ifndef __VECTOR_HPP__
#define __VECTOR_HPP__

#include <iostream>

#define uint unsigned int


class Point;


class Vector {
public:
    double x;
    double y;
    double z;
public:
    Vector() { x = 0; y = 0; z = 0; };
    Vector(double _x) : x(_x), y(_x), z(_x) {};
    Vector(double _x, double _y, double _z) : x(_x), y(_y), z(_z) {};
    Vector(const Point& p) : Vector(p.x, p.y, p.z) {};

    double operator[](uint id) const;
    double& operator[](uint id);

    Vector operator+(const Vector& vec) const;
    Vector operator-() const { return Vector(-x, -y, -z); };
    Vector operator-(const Vector& vec) const { return (*this) + (-vec); };
    Vector operator*(double a) const { return Vector(x * a, y * a, z * a); };
    Vector operator/(double a) const { return Vector(x / a, y / a, z / a); };

    double SquaredNorm() const { return x * x + y * y + z * z; };
    double Norm() const { return std::sqrt(SquaredNorm()); };
    Vector Normalized() const { return (*this) / Norm(); };
};



// Vector Inline Functions -----------------------------------------------------


inline Vector operator*(double a, const Vector& vec) { return vec * a; }


inline std::ostream& operator<<(std::ostream& os, const Vector vec) {
    os << "(" << vec.x << ", " << vec.y << ", " << vec.z << ")";
    return os;
}


inline Vector Normalize(const Vector& vec) {
    return vec / vec.Norm();
}


inline Vector Abs(const Vector& vec) {
    return Vector(std::abs(vec.x), std::abs(vec.y), std::abs(vec.z));
}


inline double Dot(const Vector& v1, const Vector& v2) {
    return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

inline double AbsDot(const Vector& v1, const Vector& v2) {
    return std::abs(Dot(v1, v2));
}


inline Vector Cross(const Vector& v1, const Vector& v2) {
    return Vector((v1.y * v2.z) - (v1.z * v2.y),
                      (v1.z * v2.x) - (v1.x * v2.z),
                      (v1.x * v2.y) - (v1.y * v2.x));
}


double MinComponent(const Vector& vec) {
    return std::min(vec.x, std::min(vec.y, vec.z));
}

double MaxComponent(const Vector& vec) {
    return std::max(vec.x, std::max(vec.y, vec.z));
}

uint MaxDimension(const Vector& vec) {
    return (vec.x > vec.y) ? ((vec.x > vec.z) ? 0 : 2) : ((vec.y > vec.z) ? 1 : 2);
}

Vector Min(const Vector& v1, const Vector& v2) {
    return Vector(std::min(v1.x, v2.x), std::min(v1.y, v2.y), std::min(v1.z, v2.z));
}

Vector Max(const Vector& v1, const Vector& v2) {
    return Vector(std::max(v1.x, v2.x), std::max(v1.y, v2.y), std::max(v1.z, v2.z));
}


Vector Permute(const Vector& vec, uint xid, uint yid, uint zid) {
    return Vector(vec[xid], vec[yid], vec[zid]);
}


inline void CoordinateSystem(const Vector& v1, Vector& v2, Vector& v3) {
    if (std::abs(v1.x) > std::abs(v1.y))
        v2 = Vector(-v1.z, 0, v1.x).Normalized();
    else v2 = Vector(0, v1.z, -v1.y).Normalized();
    v3 = Cross(v1, v2);
}


#endif