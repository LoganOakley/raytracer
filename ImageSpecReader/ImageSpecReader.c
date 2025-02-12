#include "ImageSpecReader.h"
#include <stdlib.h>
#include <string.h>

void handleCommmand(ImageSpec *spec, char *command, int argc, char **argv){
	SWITCH(command, "imsize"){
		spec->width = atoi(argv[0]);
		spec->height = atoi(argv[1]);
	}
	CASE(command, "bkgcolor"){
		color bkgcolor = {atof(argv[0]), atof(argv[1]), atof(argv[2])};
		spec->bkgcolor = bkgcolor;
	}
	CASE(command, "eye"){
		point origin = {atof(argv[0]),atof(argv[1]),atof(argv[2])};
		spec->origin = origin;
	}
	CASE(command, "viewdir"){
		point viewdir = {atof(argv[0]),atof(argv[1]),atof(argv[2])};
		spec->viewdir = viewdir;
	}
	CASE(command, "updir"){
		point updir = {atof(argv[0]),atof(argv[1]),atof(argv[2])};
		spec->updir = updir;
	}
	CASE(command, "vfov"){
		spec->vfov = atof(argv[0]);
	}
	CASE(command, "mtlcolor"){
		color matcolor = {atof(argv[0]),atof(argv[1]),atof(argv[2])};
		material mat = {matcolor};
		spec->materialCount++;
		spec->materials = realloc(spec->materials, spec->materialCount * sizeof(material));
		spec->materials[spec->materialCount -1] = mat;
	}
	CASE(command, "sphere"){
		sphere obj = {{atof(argv[0]),atof(argv[1]),atof(argv[2])},atof(argv[3]), spec->materialCount-1};
		spec->sphereCount++;
		spec->spheres = realloc(spec->spheres, spec->sphereCount * sizeof(sphere));
		spec->spheres[spec->sphereCount-1] = obj;
	}
	DEFAULT{
		printf("Unknown command: %s\n", command);
	}
}

ImageSpec *readImageSpec(FILE *specFile){

	ImageSpec *spec = malloc(sizeof(ImageSpec));
	spec->sphereCount=0;
	spec->spheres = malloc(0);
	spec->materialCount=0;
	spec->materials = malloc(0);
	char buff[255];

	while(fgets(buff, 255, specFile) != NULL){
		char *command = strtok(buff, " \n");
		if(command == NULL){
			continue;
		}
		char *arg = strtok(NULL, " \n");
		int count = 0;
		char **args = malloc(count*sizeof(char*));
		while(arg != NULL){
			count++;
			args = realloc(args, count * sizeof(char*));
			args[count-1] = arg;
			arg = strtok(NULL, " \n");
		}
		handleCommmand(spec, command, count, args);
		free(args);
	}
	
	for(int i =  0; i < spec->sphereCount; i++){
		sphere s = spec->spheres[i];
		color matColor = spec->materials[s.matIndex].matColor; 
	}
	return spec;
}

