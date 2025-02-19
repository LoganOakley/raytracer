#ifndef VECTOR
#define  VECTOR
#include "../ImageSpecReader/ImageSpecReader.h"
point crossProduct(point p1, point p2);
point scale(double sFactor, point p);
point sumPoints(int count, ...);
double length(point p);
point normalize(point p);
double dot(point p1, point p2);
#endif
