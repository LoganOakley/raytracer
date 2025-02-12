#include <stdarg.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdio.h>
#include "ImageSpecReader/ImageSpecReader.h"
#include "Ray/ray.h"

#define MAX_PIXEL_SIZE 13
#define MAX_HEADER_LENGTH 20

void printPoint(point p){
	printf("x: %.2f, y: %.2f, z: %.2f\n", p.x, p.y, p.z);
};

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

point normalize(point p){
	double length = sqrt((p.x*p.x)+(p.y*p.y)+(p.z*p.z));
	point normalized = scale(1/length, p);
	return normalized;
}


int main(int argc, char* argv[]){
	if(argc < 2){
		printf("No image spec given.\n");
		return 1;
	}
	char *inFilePath = argv[1];

	FILE *inFile;
	inFile = fopen(inFilePath, "r");
	ImageSpec *spec = readImageSpec(inFile);


/* use image spec to compute required values:
	* w, u, v
	* h, v
	* ul, ur, ll, lr
	* ^h, ^v
	* rays
	* intersections
	* colors

* create picture outputting into an outfile
*/
	point w = normalize(scale(-1, spec->viewdir));
	point u = normalize(crossProduct(spec->updir, w));
	point v = normalize(crossProduct(w, u));

	point n = normalize(spec->viewdir);

	int d = 1;

	double theta = M_PI*spec->vfov/180;
	double vwHeight = 2*d*tan(theta/2);
	double vwWidth = spec->width * vwHeight/spec->height;

	point ul = sumPoints(4, spec->origin, scale(d, n), scale(-vwWidth/2, u),scale(vwHeight/2, v));
	point ur = sumPoints(4, spec->origin, scale(d, n), scale(vwWidth/2, u),scale(vwHeight/2, v));
	point ll = sumPoints(4, spec->origin, scale(d, n), scale(-vwWidth/2, u),scale(-vwHeight/2, v));
	point lr = sumPoints(4, spec->origin, scale(d, n), scale(vwWidth/2, u),scale(-vwHeight/2, v));

	point hDelta = scale( 1.0/(spec->width-1), (sumPoints(2, ur, scale(-1, ul))));
	point vDelta = scale( 1.0/(spec->height-1), (sumPoints(2, ll, scale(-1, ul))));

	//create output file path by stripping the current extension and replacing with .ppm
	char outPath[strlen(inFilePath) + 2]; // outpath is 2 larger incase infile has a single character extension
	strcpy(outPath, inFilePath);
	char *ext = strrchr(outPath, '.');
	*ext = '\0';
	strcat(outPath, ".ppm");

	
	FILE *outFile;
	outFile = fopen(outPath, "w");
	if(outFile == NULL){
		printf("Issue creating image file\n");
		return 1;
	}

	char *header=malloc(MAX_HEADER_LENGTH);// create and write header to files
	snprintf(header, MAX_HEADER_LENGTH, "P3 %d %d 255\n", spec->width, spec->height);
	fputs(header, outFile);
	free(header);

	char *pixel=malloc(MAX_PIXEL_SIZE);

	for( int y = 0; y < spec->height; y++){
		for( int x=0; x<spec->width; x++){
			point rayEnd = sumPoints(3, ul, scale(x, hDelta), scale(y, vDelta));
			point raydir = normalize(sumPoints(2, rayEnd, scale(-1, spec->origin)));
		       	color c = TraceRay(spec, (ray){spec->origin,raydir});
			snprintf(pixel, MAX_PIXEL_SIZE, "%.0f %.0f %.0f\n", c.r*255, c.g*255, c.b*255);
			fputs(pixel, outFile);
		}
	}

	free(pixel);
	fclose(outFile);
	
	return 0;
}
