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


// EOF