#include "point2.hpp"

Point2::Point2() {
    x = 0;
    y = 0;
}

Point2::Point2(const Point& pt) {
    x = pt.x;
    y = pt.y;
}

Point2::Point2(double _x, double _y) {
    x = _x;
    y = _y;
}

double Point2::Norm() const {
    return sqrt(x * x + y * y);
}

void Point2::Normalize() {
    x /= this->Norm();
    y /= this->Norm();
}

void Point2::Rotate(double angle) {
    
}

Point Point2::ToPoint() const {
    return Point(x, y, 0);
}


std::ostream& operator<<(std::ostream& os, const Point2& p) {
    os << "(" << p.x << " ; " << p.y << ")";
    return os;
}
Point2 operator+(const Point2& p1, const Point2& p2) {
    return Point2(p1.x + p2.x, p1.y + p2.y);
}
Point2 operator-(const Point2& p1, const Point2& p2) {
    return Point2(p1.x - p2.x, p1.y - p2.y);
}
Point2 operator*(const double& a, const Point2& p) {
    return Point2(a * p.x, a * p.y);
}
Point2 operator*(const Point2& p, const double& a) {
    return Point2(a * p.x, a * p.y);
}
Point2 operator/(const Point2& p, const double& a) {
    return Point2(p.x / a, p.y / a);
}

double det(const Point2& p1, const Point2& p2) {
    return p1.x * p2.y - p1.y * p2.x;
}

//EOF