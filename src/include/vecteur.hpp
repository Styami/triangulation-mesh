#ifndef __VECTEUR__
#define __VECTEUR__

#include "point.hpp"
#include <ostream>
struct Vecteur {
    Vecteur();
    Vecteur(int i);
    Vecteur(const Point& a, const Point& b);
    Vecteur(const float a, const float b, const float c);
    
    Vecteur operator/(const float n) const; 
    Vecteur operator*(const float n) const;
    Vecteur operator-() const;
    inline Vecteur operator+(const Vecteur& v) const;
    inline friend Vecteur operator-(const Point& p1, const Point& p2);
    inline friend Point operator+(const Point& p, const Vecteur& v);
    inline friend Point operator+(const Vecteur& v, const Point& p);
    friend Vecteur operator*(const float n, const Vecteur& v);
    friend std::ostream& operator<<(std::ostream& os, const Vecteur& v);

    friend float dot(const Vecteur& a, const Vecteur& b);
    friend Vecteur normalize(const Vecteur& vec);
    friend float length(const Vecteur& a);
    friend Vecteur cross(const Vecteur& vec1, const Vecteur& vec2);

    float x, y, z;
};

Vecteur operator-(const Point& p1, const Point& p2) {
    return {p1.x - p2.x, p1.y - p2.y, p1.z - p2.z};
}

Vecteur Vecteur::operator+(const Vecteur& v) const {
    return {x + v.x, y + v.y, z + v.z};
}

Point operator+(const Point& p, const Vecteur& v) {
    return {p.x + v.x, p.y + v.y, p.z + v.z};
}

Point operator+(const Vecteur& v, const Point& p) {
    return p + v;
 }

#endif