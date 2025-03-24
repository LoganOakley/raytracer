#ifndef COLORS
#define COLORS
typedef struct {
	double r;
	double g;
	double b;
	double refraction;
} color;
void printColor(color c);
color scaleColor(double sFactor, color c);
color sumColors(int count, ...);
#endif
