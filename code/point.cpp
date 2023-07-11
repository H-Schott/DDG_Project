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

//EOF