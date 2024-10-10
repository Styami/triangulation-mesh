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
    friend Vecteur operator*(const float n, const Vecteur& v);
    friend std::ostream& operator<<(std::ostream& os, const Vecteur& v);

    friend float dot(const Vecteur& a, const Vecteur& b);
    friend Vecteur normalize(const Vecteur& vec);
    friend float length(const Vecteur& a);
    friend Vecteur cross(const Vecteur& vec1, const Vecteur& vec2);

    float x, y, z;
};

#endif