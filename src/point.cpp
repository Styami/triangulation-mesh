#include "include/point.hpp"

Point::Point(int i) :
    x(i),
    y(i),
    z(i)
{}

Point::Point() :
    x(0),
    y(0),
    z(0)
{}

Point::Point(float a, float b, float c) :
    x(a),
    y(b),
    z(c)
{}

Point Point::operator/(const float f) {
    return Point(x/f, y/f, z/f);
}

std::ostream& operator<<(std::ostream& os, const Point& point) {
    os << point.x << ' ' << point.y << ' ' << point.z;
    return os;
}

