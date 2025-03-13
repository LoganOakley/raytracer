#ifndef TEXTURE
#define TEXTURE
#include <stdio.h>
#include "../Color/color.h"

typedef struct{
	int width;
	int height;
	color colors[];
}texture;

texture *ReadTexture(FILE*);

#endif
