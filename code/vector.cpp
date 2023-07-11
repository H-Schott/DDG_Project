#include "vector.hpp"
#include "point.hpp"


Vector::Vector() {
    x = 0;
    y = 0;
    z = 0;
};

Vector::Vector(double _x) : x(_x), y(_x), z(_x) {

};

Vector::Vector(double _x, double _y, double _z) : x(_x), y(_y), z(_z) {

};

Vector::Vector(const Point& p) : Vector(p.x, p.y, p.z) {

};

Vector Vector::operator-() const {
    return Vector(-x, -y, -z);
};

Vector Vector::operator-(const Vector& vec) const {
    return (*this) + (-vec);
};

Vector Vector::operator*(double a) const {
    return Vector(x * a, y * a, z * a);
};

Vector Vector::operator/(double a) const {
    return Vector(x / a, y / a, z / a);
};

double Vector::SquaredNorm() const {
    return x * x + y * y + z * z;
};

double Vector::Norm() const {
    return std::sqrt(SquaredNorm());
};

Vector Vector::Normalized() const {
    return (*this) / Norm();
};

double Vector::operator[](uint id) const {
    if (id == 0) return x;
    if (id == 1) return y;
    return z;
}

double& Vector::operator[](uint id) {
    if (id == 0) return x;
    if (id == 1) return y;
    return z;
}


Vector Vector::operator+(const Vector& vec) const {
    return Vector(x + vec.x, y + vec.y, z + vec.z);
}


// EOF