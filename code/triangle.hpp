#ifndef __TRIANGLE_HPP__
#define __TRIANGLE_HPP__

#include "point.hpp"
#include "vector.hpp"

class Triangle {
public:
    Point a;
    Point b;
    Point c;

    Triangle();
    Triangle(Point a, Point b, Point c);

    Vector Normal(bool unit=true) const;
    double Area() const;
};


#endif