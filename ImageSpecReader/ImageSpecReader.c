#include "ImageSpecReader.h"
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
	CASE(command, "viewdir"){
		if(argc < 3){
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
		if(argc < 3){
			printf("Incorrect arguments for mtlcolor.\n");
			exit(1);
		}
		color matcolor = {atof(argv[0]),atof(argv[1]),atof(argv[2])};
		material mat = {matcolor};
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

	//list of required commands
	char *requiredCommands[NUM_REQUIRED] = REQUIRED;
	//flags to track which required commands have been used atleast once
	unsigned char usedCommands = 0b0;
	ImageSpec *spec = malloc(sizeof(ImageSpec));
	spec->sphereCount=0;
	spec->spheres = malloc(0);
	spec->materialCount=0;
	spec->materials = malloc(0);
	char buff[255];

	while(fgets(buff, 255, specFile) != NULL){
		// commands are the first tokens found splitting a line on spaces and newlines
		// if command is null line is blank so we skip the line
		char *command = strtok(buff, " \n");
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

