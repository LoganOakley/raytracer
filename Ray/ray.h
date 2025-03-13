#ifndef RAY
#define RAY
#include "../ImageSpecReader/ImageSpecReader.h"
#include "../Color/color.h"
typedef struct{
	point origin;
	point dir;
} ray;

typedef struct{
	double distance;
	point iPoint;
	int objIndex;
	point barycentCoords;
}intersectionData;

color TraceRay(ImageSpec *p, ray ray);
color ShadeRay(ImageSpec *spec,  ray *r, intersectionData *intersection);

#endif
