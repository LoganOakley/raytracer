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
	color specularColor;
	double ambientStrength;
	double diffuseStrength;
	double specularStrength;
	int specularFallOff;
} material;

typedef struct{
	point center;
	double radius;
} sphere;

typedef struct{
	int points[3];
	int textures[3];
	int normals[3];
} triangle;

typedef union {
	sphere s;
	triangle t;
} shape;

typedef struct {
	shape shape;
	int matIndex;
	unsigned int shapeType; // the shape type is the number of vertices on the shape, 0 for sphere 3 for triangle
} object;

typedef struct{
	point loc;
	unsigned char type;
	double intensity;
	unsigned char attenuated;
	double c1;
	double c2;
	double c3;
} light;

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
	int objectCount;
	object *objects;
	int lightCount;
	light *lights;
	int vertexCount;
	point *vertices;
	int normCount;
	point *norms;
	int textureCount;
	point *textureCoords;
} ImageSpec;

void handleCommand(ImageSpec*, char*, int, char**);
ImageSpec *readImageSpec(FILE*);
void printPoint(point p);
void printColor(color c);
color scaleColor(double sFactor, color c);
color sumColors(int count, ...);
#endif
