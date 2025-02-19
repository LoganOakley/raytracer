#ifndef RAY
#define RAY
#include "../ImageSpecReader/ImageSpecReader.h"
typedef struct{
	point origin;
	point dir;
} ray;

color TraceRay(ImageSpec *p, ray ray);
color ShadeRay(ImageSpec *spec, int sphereIndex, ray *r, double intersectionDistance);

#endif
