#ifndef __TRIANGLE2_HPP__
#define __TRIANGLE2_HPP__


class Triangle2
{
public:
    Point2 a;
    Point2 b;
    Point2 c;

    Triangle2();
    Triangle2(Point2 p1, Point2 p2, Point2 p3);

    double Area() const;
    Point2 Barycentre() const;

    // tests
    bool TestTrigo() const;
    bool Contains(const Point2&) const;
};


#endif
