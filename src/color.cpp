#include "include/color.hpp"

Color::Color() : r(255), g(255), b(255) {}

Color::Color(unsigned int a, unsigned int b, unsigned int c) : r(a), g(b), b(c) {}

unsigned int Color::getRed() {return r;}
unsigned int Color::getGreen() {return g;}
unsigned int Color::getBlue() {return b;}