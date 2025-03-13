#include "color.h"
#include <stdarg.h>
#include <stdio.h>

void printColor(color c){
	printf("r: %.2f, g: %.2f, b: %.2f\n", c.r, c.g, c.b);
};

color scaleColor(double sFactor, color c){
	color scaled = {c.r * sFactor, c.g * sFactor, c.b * sFactor};
	return scaled;
}

color sumColors(int count, ...){
	va_list colors;
	va_start(colors, count);
	color sum = {0, 0, 0};
	for(int i=0; i < count; i++){
		color c = va_arg(colors, color);
		sum.r += c.r;
		sum.g += c.g;
		sum.b += c.b;
	}
	va_end(colors);
	return sum;
};
