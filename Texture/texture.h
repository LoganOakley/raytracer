#ifndef TEXTURE
#define TEXTURE
#include <stdio.h>
#include "../ImageSpecReader/ImageSpecReader.h"


typedef struct{
	int width;
	int height;
	color *colors[];
}texture;

texture *ReadTexture(FILE*);

#endif
