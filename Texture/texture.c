
#include "texture.h"
#include <stdlib.h>
#include <stdio.h>
texture *ReadTexture(FILE* textureFile){
	char buff[255];
	
	
	if(fgets(buff, 255, textureFile) == NULL){
		
		printf("Texture file not compatable\n");
		exit(1);
	}
	
	//TODO: tokenize header to get width and height
	// add width and height to texture variable
	// create a 2D color array c[height][width] to store color data
	// point to the 2D array in the texture variable
	// return pointer to texture
	
	
	while(fgets(buff, 255, textureFile) != NULL){
	}
	texture *t = malloc(sizeof(texture));
	return t;
}
