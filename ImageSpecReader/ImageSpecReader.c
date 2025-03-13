#include "ImageSpecReader.h"
#include "../Vector/vector.h"
#include "../Texture/texture.h"
#include "../Color/color.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define NUM_REQUIRED 6
#define REQUIRED {"imsize", "eye", "viewdir", "vfov", "updir", "bkgcolor"}
void handleCommmand(ImageSpec *spec, char *command, int argc, char **argv){
	//use the command to handle what to do with each argument. Also catches errors around argument counts
	SWITCH(command, "imsize"){
		if(argc < 2){
			printf("Incorrect arguments for imsize.\n");
			exit(1);
		}
		spec->width = atoi(argv[0]);
		spec->height = atoi(argv[1]);
	}
	CASE(command, "bkgcolor"){
		if(argc < 3){
			printf("Incorrect arguments for bkgcolor.\n");
			exit(1);
		}
		color bkgcolor = {atof(argv[0]), atof(argv[1]), atof(argv[2])};
		spec->bkgcolor = bkgcolor;
	}
	CASE(command, "eye"){
		if(argc < 3){
			printf("Incorrect arguments for eye.\n");
			exit(1);
		}
		point origin = {atof(argv[0]),atof(argv[1]),atof(argv[2])};
		spec->origin = origin;
	}
	CASE(command, "viewdir"){ if(argc < 3){
			printf("Incorrect arguments for viewdir.\n");
			exit(1);
		}
		point viewdir = {atof(argv[0]),atof(argv[1]),atof(argv[2])};
		spec->viewdir = viewdir;
	}
	CASE(command, "updir"){
		if(argc < 3){
			printf("Incorrect arguments for updir.\n");
			exit(1);
		}
		point updir = {atof(argv[0]),atof(argv[1]),atof(argv[2])};
		spec->updir = updir;
	}
	CASE(command, "vfov"){
		if(argc < 1){
			printf("Incorrect arguments for vfov.\n");
			exit(1);
		}
		spec->vfov = atof(argv[0]);
	}
	CASE(command, "mtlcolor"){
		if(argc < 10){
			printf("Incorrect arguments for mtlcolor.\n");
			exit(1);
		}
		color matcolor = {atof(argv[0]),atof(argv[1]),atof(argv[2])};
		color specColor = {atof(argv[3]),atof(argv[4]),atof(argv[5])};
		material mat = {matcolor, specColor, atof(argv[6]), atof(argv[7]), atof(argv[8]), atoi(argv[9])};
		spec->materialCount++;
		spec->materials = realloc(spec->materials, spec->materialCount * sizeof(material));
		spec->materials[spec->materialCount -1] = mat;
	}
	CASE(command, "sphere"){
		if(argc < 4){
			printf("Incorrect arguments for sphere.\n");
			exit(1);
		}
		if(spec->materialCount < 1){
			printf("No material set before sphere.\n");
			exit(1);
		}
		shape s;
		s.s = (sphere){{atof(argv[0]),atof(argv[1]),atof(argv[2])},atof(argv[3])};
		object obj = {s, spec->materialCount-1, 0, spec->textureCount-1};

		spec->objectCount++;
		spec->objects = realloc(spec->objects, spec->objectCount * sizeof(object));
		spec->objects[spec->objectCount-1] = obj;
	}
	CASE(command, "light"){	
		if(argc < 5){
			printf("Incorrect arguments for light\n");
			exit(1);
		}
		light l = {{atof(argv[0]), atof(argv[1]),atof(argv[2])},atoi(argv[3]),atof(argv[4])};
		spec->lightCount++;
		spec->lights = realloc(spec->lights, spec->lightCount * sizeof(light));
		spec->lights[spec->lightCount-1] = l;
	}
	CASE(command, "attlight"){	
		if(argc < 8){
			printf("Incorrect arguments for attlight\n");
			exit(1);
		}
		light l = {{atof(argv[0]), atof(argv[1]),atof(argv[2])},atoi(argv[3]),atof(argv[4]), 1, atof(argv[5]), atof(argv[6]), atof(argv[7])};
		spec->lightCount++;
		spec->lights = realloc(spec->lights, spec->lightCount * sizeof(light));
		spec->lights[spec->lightCount-1] = l;
	}
	CASE(command, "v"){
		if(argc<3){
			printf("Incorrect arguments for vertex\n");
			exit(1);
		}
		point vert = {atof(argv[0]), atof(argv[1]), atof(argv[2])};
		spec->vertexCount++;
		spec->vertices = realloc(spec->vertices, spec->vertexCount * sizeof(point));
		spec->vertices[spec->vertexCount-1] = vert;
	}
	CASE(command, "vn"){
		if(argc<3){
			printf("Incorrect arguments for vertex normal\n");
			exit(1);
		}
		point normal = {atof(argv[0]), atof(argv[1]), atof(argv[2])};
		spec->normCount++;
		spec->norms = realloc(spec->norms, spec->normCount * sizeof(point));
		spec->norms[spec->normCount-1] = normal;
	}
	CASE(command, "vt"){
		if(argc<2){
			printf("Incorrect arguments for vertex texture\n");
			exit(1);
		}
		point textureCoord = {atof(argv[0]), atof(argv[1]), 0};
		spec->textureCoordCount++;
		spec->textureCoords = realloc(spec->textureCoords, spec->textureCoordCount * sizeof(point));
		spec->textureCoords[spec->textureCoordCount-1] = textureCoord;
	}
	CASE(command, "f"){
		if(argc != 3){
			printf("Incorrect arguments for triangle\n");
			exit(1);
		}
		if(spec->materialCount == 0){
			printf("No material set before triangle.\n");
			exit(1);
		}

		shape shape;

		//handle case where there arent two slashs
		for (int i = 0; i<argc; i++) {
			char *string = malloc(sizeof(argv[i]));
			strcpy(string,argv[i]); 
			char *tok = strsep(&string, "/"); 
			shape.t.points[i] = atoi(tok)-1;
			tok = strsep(&string, "/"); 
			if(tok != NULL){
				shape.t.textures[i] = atoi(tok)-1;
				tok = strsep(&string, "/"); 
			}else {
				shape.t.textures[i] = -1;
			}
			if(tok != NULL){
				shape.t.normals[i] = atoi(tok)-1;
			}else {
				shape.t.normals[i] = -1;
			}
		}
		point p1 = spec->vertices[shape.t.points[0]];
		point p2 = spec->vertices[shape.t.points[1]];
		point p3 = spec->vertices[shape.t.points[2]];

		shape.t.basisI = sumPoints(2, p2, scale(-1, p1));
		shape.t.basisJ = sumPoints(2, p3, scale(-1, p1));
		shape.t.norm = normalize(crossProduct(shape.t.basisI, shape.t.basisJ));

		shape.t.d11 = dot(shape.t.basisI, shape.t.basisI);
		shape.t.d12 = dot(shape.t.basisI, shape.t.basisJ);
		shape.t.d22 = dot(shape.t.basisJ, shape.t.basisJ);
		shape.t.det = shape.t.d11*shape.t.d22 - shape.t.d12*shape.t.d12;


		object obj = {shape, spec->materialCount-1, 3, spec->textureCount-1};
		spec->objectCount++;
		spec->objects = realloc(spec->objects, spec->objectCount * sizeof(object));
		spec->objects[spec->objectCount-1] = obj;
	}
	CASE(command, "texture"){
		FILE *textureFile;
		textureFile = fopen(argv[0], "r");
		if(textureFile == NULL){
			printf("Failed opening texture\n");
			exit(1);
		}
		texture *t = ReadTexture(textureFile);
		//TODO: put texture into image spec. Then add texture index to objects to use for coloring

		spec->textureCount++;
		spec->textures = realloc(spec->textures, spec->textureCount * sizeof(texture));
		spec->textures[spec->textureCount-1] = t;
		fclose(textureFile);
	}
	DEFAULT{
		printf("Unknown command: %s\n", command);
	}
}

ImageSpec *readImageSpec(FILE *specFile){

	//list of required commands
	char *requiredCommands[NUM_REQUIRED] = REQUIRED;
	//flags to track which required commands have been used atleast once
	unsigned char usedCommands = 0b0;
	ImageSpec *spec = malloc(sizeof(ImageSpec));
	spec->objectCount=0;
	spec->objects = malloc(0);
	spec->materialCount=0;
	spec->materials = malloc(0);
	spec->lightCount = 0;
	spec->lights = malloc(0);
	spec->vertexCount = 0;
	spec->vertices = malloc(0);
	spec->normCount = 0;
	spec->norms = malloc(0);
	spec->textureCoordCount = 0;
	spec->textureCoords = malloc(0);
	spec->textureCount = 0;
	spec->textures = malloc(0);
	char buff[255];

	while(fgets(buff, 255, specFile) != NULL){
		char *buffSvPtr;
		// commands are the first tokens found splitting a line on spaces and newlines
		// if command is null line is blank so we skip the line
		char *command = strtok_r(buff, " \n\r", &buffSvPtr);
		if(command == NULL){
			continue;
		}

		for(int i=0; i < NUM_REQUIRED; i++){
			if (strcmp(command,requiredCommands[i]) == 0){
				usedCommands |= (1 << i);
				break;
			}
		}

		// we grow and array to hold all of the arguments for a command dynamically (could be improved with a larger scale factor \_(0.0)_/)
		char *arg = strtok_r(NULL, " \n", &buffSvPtr);
		int count = 0;
		char **args = malloc(count*sizeof(char*));
		while(arg != NULL){
			count++;
			args = realloc(args, count * sizeof(char*));
			args[count-1] = arg;
			arg = strtok_r(NULL, " \n", &buffSvPtr);
		}
		handleCommmand(spec, command, count, args);
		free(args);
	}	

	//if not all requred commands have been used, check throught the flags, displaying the commands that were missed
	if( usedCommands != (1<<NUM_REQUIRED)-1){
		printf("Required parameters not set: ");
		for(int i = 0; i < NUM_REQUIRED; i++){
			if(!(usedCommands & (1<<i))){
				printf("%s ", requiredCommands[i]);
			}
		}
		printf("\n");
		exit(1);
	}
	
	return spec;
}

// helper functions
void printPoint(point p){
	printf("x: %.2f, y: %.2f, z: %.2f\n", p.x, p.y, p.z);
};

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
