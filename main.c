#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdio.h>
#include "ImageSpecReader/ImageSpecReader.h"
#include "Ray/ray.h"
#include "Vector/vector.h"

#define MAX_PIXEL_SIZE 13
#define MAX_HEADER_LENGTH 20

void render(ImageSpec *spec, char* outPath){
	// w= -viewdir, u = cross of updir and view dir, v = cross of w and u
	// normalized for calculations
	point w = normalize(scale(-1, spec->viewdir));
	point u = normalize(crossProduct(spec->updir, w));
	point v = normalize(crossProduct(w, u));

	// n is the unit vector in the view direction
	point n = normalize(spec->viewdir);

	// using d = 1 implicitly as it is only used in multiplication

	//convert fov to radians for math.h tan function
	double theta = M_PI*spec->vfov/180;
	double vwHeight = 2*tan(theta/2);
	// use the aspect ratio to get view window width
	double vwWidth = spec->width * vwHeight/spec->height;

	//define corners of view window
	point ul = sumPoints(4, spec->origin, n, scale(-vwWidth/2, u),scale(vwHeight/2, v));
	point ur = sumPoints(4, spec->origin, n, scale(vwWidth/2, u),scale(vwHeight/2, v));
	point ll = sumPoints(4, spec->origin, n, scale(-vwWidth/2, u),scale(-vwHeight/2, v));

	// use pixel dimensions and viewwindow corners get our transformation vectors
	point hDelta = scale( 1.0/(spec->width-1), (sumPoints(2, ur, scale(-1, ul))));
	point vDelta = scale( 1.0/(spec->height-1), (sumPoints(2, ll, scale(-1, ul))));

	FILE *outFile;
	outFile = fopen(outPath, "w");
	if(outFile == NULL){
		printf("Issue creating image file\n");
		exit(1);
	}

	// create header
	char *header=malloc(MAX_HEADER_LENGTH);// create and write header to files
	snprintf(header, MAX_HEADER_LENGTH, "P3 %d %d 255\n", spec->width, spec->height);
	fputs(header, outFile);
	free(header);

	char *pixel=malloc(MAX_PIXEL_SIZE);
	// get the ray for each point and trace it for the color to set the pixel
	for( int y = 0; y < spec->height; y++){
		for( int x=0; x<spec->width; x++){
			//create unit ray with origin at the eye and pointing to pixel (x,y)
			point rayEnd = sumPoints(3, ul, scale(x, hDelta), scale(y, vDelta));
			point raydir = normalize(sumPoints(2, rayEnd, scale(-1, spec->origin)));
		       	color c = TraceRay(spec, (ray){spec->origin,raydir});
			snprintf(pixel, MAX_PIXEL_SIZE, "%.0f %.0f %.0f\n", c.r*255, c.g*255, c.b*255);
			fputs(pixel, outFile);
		}
	}
	free(pixel);
	fclose(outFile);
}

int main(int argc, char* argv[]){
	if(argc < 2){
		printf("No image spec given.\n");
		return 1;
	}
	char *inFilePath = argv[1];

	FILE *inFile;
	//open file ensuring it exists
	inFile = fopen(inFilePath, "r");
	if(inFile == NULL){
		printf("Failed opening file: %s\n", inFilePath);
		exit(1);
	}
	ImageSpec *spec = readImageSpec(inFile);

	fclose(inFile);
	if(length(crossProduct(spec->updir, spec->viewdir)) == 0){
		printf("The updir and viewdir are co-linear, unable to render.\n");
		exit(1);
	}

	//create output file path by stripping the current extension and replacing with .ppm
	char outPath[strlen(inFilePath) + 2]; // outpath is 2 larger incase infile has a single character extension
	strcpy(outPath, inFilePath);
	char *ext = strrchr(outPath, '.');
	*ext = '\0';
	strcat(outPath, ".ppm");

	render(spec, outPath);
	return 0;
}

