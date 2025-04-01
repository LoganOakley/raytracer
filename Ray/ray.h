#ifndef RAY
#define RAY
#include "../ImageSpecReader/ImageSpecReader.h"
#include "../Color/color.h"
#include "../Stack/stack.h"
typedef struct{
	point origin;
	point dir;
	colorStack *traveralStack;
} ray;

typedef struct{
	double distance;
	point iPoint;
	int objIndex;
	point barycentCoords;
}intersectionData;

color TraceRay(ImageSpec *p, ray ray, int iteration);
color ShadeRay(ImageSpec *spec,  ray *r, intersectionData *intersection, int iteration);

#endif
