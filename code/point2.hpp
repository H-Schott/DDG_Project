#ifndef __POINT2_HPP__
#define __POINT2_HPP__

#include "point.hpp"

class Point2 {
public:
    double x;
    double y;

    Point2();
    Point2(const Point&);
    Point2(double, double);

    double Norm() const;
    void Normalize();
    void Rotate(double);

    Point ToPoint() const;

};

std::ostream& operator<<(std::ostream& os, const Point2&);
Point2 operator+(const Point2&, const Point2&);
Point2 operator-(const Point2&, const Point2&);
Point2 operator*(const double&, const Point2&);
Point2 operator*(const Point2&, const double&);
Point2 operator/(const Point2&, const double&);

double det(const Point2&, const Point2&);


#endif