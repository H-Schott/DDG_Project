#include "point.hpp"
#include "vector.hpp"

Point::Point() { 
    x = 0;
    y = 0;
    z = 0;
};

Point::Point(double _x, double _y, double _z) {
    x = _x;
    y = _y;
    z = _z;
};

Point::Point(const Point& pt) : x(double(pt.x)), y(double(pt.y)), z(double(pt.z)) {

};

double Point::operator[](uint id) const {
    if(id == 0) return x;
    if(id == 1) return y;
    return z;
};

double& Point::operator[](uint id) {
    if(id == 0) return x;
    if(id == 1) return y;
    return z;
};

Point Point::operator+(const Vector& vec) const {
    return Point(x + vec.x, y + vec.y, z + vec.z);
};

Point Point::operator+(const Point& pt) const {
    return Point(x + pt.x, y + pt.y, z + pt.z);
};

Point Point::operator-(const Vector& vec) const {
    return (*this) + (-vec);
};

Vector Point::operator-(const Point& pt) const {
    return Vector(x - pt.x, y - pt.y, z - pt.z);
};

Point Point::operator*(double a) const {
    return Point(a * x, a * y, a * z);
};

Point Point::operator/(double a) const {
    return Point(x / a, y / a, z / a);
};

inline double Distance(const Point& p1, const Point& p2) {
    return (p1 - p2).Norm();
}

inline double SquaredDistance(const Point& p1, const Point& p2) {
    return (p1 - p2).SquaredNorm();
}

Point Lerp(double t, const Point& p0, const Point& p1) {
    return (1 - t) * p0 + t * p1;
}

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

//EOF