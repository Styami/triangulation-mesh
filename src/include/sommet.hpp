#ifndef __SOMMET__
#define __SOMMET__

#include "point.hpp"
#include <cstddef>
#include <ostream>

typedef size_t indiceFace;

class Sommet {
    public:
        Sommet(const Point& _p);
        Sommet();
        Sommet(const Point& _p, indiceFace i);
        void setIndexeFace(indiceFace i);
        indiceFace getFaceIndex() const;
        const Point& getPoint() const;
        friend std::ostream& operator<<(std::ostream& os, const Sommet& s);
    private:
        Point p;
        indiceFace indice;
};

#endif