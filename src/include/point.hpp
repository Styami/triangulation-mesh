#ifndef __POINT__
#define __POINT__

#include <ostream>

struct Point {
    Point(float a, float b, float c);
    Point(int i);
    Point();
    Point operator/(const float f);
    friend std::ostream& operator<<(std::ostream& os, const Point& point);
    float x, y, z;
};

#endif