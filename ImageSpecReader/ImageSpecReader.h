#ifndef IMAGESPECREADER
#define	IMAGESPECREADER

#include <stdio.h>

#define  SWITCH(var, case) if(strcmp(var, case) == 0)
#define  CASE(var, case) else if(strcmp(var, case) == 0)
#define  DEFAULT else

typedef struct {
	double x;
	double y;
	double z;
} point;

typedef struct {
	double r;
	double g;
	double b;
} color;

typedef struct{
	color matColor;
} material;

typedef struct{
	point center;
	double radius;
	int matIndex;
} sphere;

typedef struct {
	int width;
	int height;
	point origin;
	point viewdir;
	point updir;
	double vfov;
	color bkgcolor;
	int materialCount;
	material *materials;
	int sphereCount;
	sphere *spheres;
} ImageSpec;

void handleCommand(ImageSpec*, char*, int, char**);
ImageSpec *readImageSpec(FILE*);

#endif
