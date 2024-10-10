#include "include/sommet.hpp"
#include <iostream>
#include <ostream>


Sommet::Sommet(const Point& _p) :
    p(_p)
{}

void Sommet::setIndexeFace(indiceFace i) {
    indice = i;
}

indiceFace Sommet::getFaceIndex() const {return indice;}

std::ostream& operator<<(std::ostream& os, const Sommet& s) {
    os << s.p;
    return os;
}

const Point& Sommet::getPoint() const {
    return p;
}