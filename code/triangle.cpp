#include "triangle.hpp"

Triangle::Triangle(Point _a, Point _b, Point _c) : a(_a), b(_b), c(_c) {}

Vector Triangle::Normal(bool unit) const {
    Vector normal = Cross(Vector(b - a), Vector(c - a));
    if (unit) return Normalize(normal);
    return normal;
}

double Triangle::Area() const {
    return 0.5 * Cross(Vector(b - a), Vector(c - a)).Norm();
}


// EOF