#include "planar_mesh.hpp"

Triangle2::Triangle2(Point2 p1, Point2 p2, Point2 p3) {
    a = p1;
    b = p2;
    c = p3;
}

double Triangle2::Area() const {
    return 0.5 * std::abs(det(a, b));
};

Point2 Triangle2::Barycentre() const {
    return (a + b + c) / 3.;
}

bool Triangle2::TestTrigo() const {
    double predicat = det(b - a, c - a);
    if (predicat == 0) {
        std::cerr << "error flat triangle\n";
        std::cout << a << " " << " " << b << " " << c << std::endl;
    }
    return predicat > 0;
}

bool Triangle2::Contains(const Point2& point) const {
    bool ABD = Triangle2(a, b, point).TestTrigo();
    bool BCD = Triangle2(b, c, point).TestTrigo();
    bool CAD = Triangle2(c, a, point).TestTrigo();

    return ABD && BCD && CAD;
}