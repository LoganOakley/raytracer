#include "vector.h"
#include <stdarg.h>
#include <math.h>

point crossProduct(point p1, point p2){
	double x = (p1.y)*(p2.z) - (p1.z)*(p2.y);
	double y = -(p1.x)*(p2.z) + (p1.z)*(p2.x);
	double z = (p1.x)*(p2.y) - (p1.y)*(p2.x);
	point cp = {x,y,z};
	return cp;
}

point scale(double sFactor, point p){
	point scaled = {p.x * sFactor, p.y * sFactor, p.z * sFactor};
	return scaled;
}

point sumPoints(int count, ...){
	va_list points;
	va_start(points, count);
	point sum = {0,0,0};
	for(int i = 0; i<count; i++){
		point p = va_arg(points, point);
		sum.x += p.x; 
		sum.y += p.y;
		sum.z += p.z;
	}

	va_end(points);
	return sum; }

double length(point p){
	return sqrt((p.x*p.x)+(p.y*p.y)+(p.z*p.z));
}

point normalize(point p){
	point normalized = scale(1/length(p), p);
	return normalized;
}

double dot(point p1, point p2){
	return p1.x * p2.x + p1.y * p2.y + p1.z * p2.z;
}

