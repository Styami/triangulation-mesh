#include "include/vecteur.hpp"
#include <cmath>

Vecteur::Vecteur(int i) : x(i), y(i), z(i) {}

Vecteur::Vecteur() : x(0), y(0), z(0) {}

Vecteur::Vecteur(const Point& a, const Point& b) :
    x(b.x - a.x),
    y(b.y - a.y),
    z(b.z - a.z)
{}

Vecteur::Vecteur(const float a, const float b, const float c) : x(a), y(b), z(c) {}

Vecteur Vecteur::operator/(const float n) const{
    return Vecteur(this->x/n, this->y/n, this->z/n);
}

Vecteur Vecteur::operator*(const float n) const {
    return {n * this->x, n * this->y, n * this->z};
}

Vecteur Vecteur::operator-() const {
    return {-this->x, -this->y, -this->z};
}

Vecteur operator*(const float n, const Vecteur& v) {
    return {n * v.x, n * v.y, n * v.z};
}

std::ostream& operator<<(std::ostream& os, const Vecteur& v) {
    os << v.x << ' ' << v.y << ' ' << v.z;
    return os;
}

float dot(const Vecteur& v1, const Vecteur& v2) {
    return (v1.x*v2.x) + (v1.y*v2.y) + (v1.z*v2.z);
}

float length(const Vecteur& a) {
    return std::sqrt(dot(a, a));
}

Vecteur normalize(const Vecteur& vec) {
    float lengthVec = length(vec);
    return vec/lengthVec;   
}

Vecteur cross(const Vecteur& vec1, const Vecteur& vec2) {
    Vecteur res;
    res.x = vec1.y * vec2.z - vec1.z * vec2.y;
    res.y =  vec1.z * vec2.x - vec1.x * vec2.z;
    res.z = vec1.x * vec2.y - vec1.y * vec2.x;
    return res;
}

