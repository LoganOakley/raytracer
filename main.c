#include <stdio.h>


typedef struct {
	int x;
	int y;
	int z;
} point;

typedef struct {
	float r;
	float g;
	float b;
}color;

typedef struct{
	point center;
	int radius;
	color *objcolor;
} sphere;

typedef struct {
	int width;
	int height;
	point origin;
	point viewdir;
	point updir;
	int vfov;
	color bkgcolor;
	color *matcolor;
	void *objects;
} ImageSpec;

int main(int argc, char* argv[]){
	if(argc < 2){
		printf("No image spec given.\n");
		return 1;
	}

	FILE *inFile;
	char buff[255];

	inFile = fopen(argv[1], "r");
	while(fgets(buff, 255, inFile) != NULL){
		printf("%s\n", buff);
	}

	return 0;
}
