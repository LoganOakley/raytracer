#include "texture.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
texture *ReadTexture(FILE* textureFile){
	char buff[255];
	
	
	if(fgets(buff, 255, textureFile) == NULL){
		
		printf("Texture file not compatable\n");
		exit(1);
	}
	
	//skip P3
	strtok(buff, " ");
	int width = atoi(strtok(NULL, " "));
	int height = atoi(strtok(NULL, " "));

	texture *t = malloc(sizeof(texture) + sizeof(color)*height * width);
	t->width = width;
	t->height = height;
	char *tok;
	int count = 0;
	//read throught texture line by line
	while(fgets(buff, 255, textureFile) != NULL){
		int cFlag = 0;
		int r, g, b;
		tok = strtok(buff, " ");
		// get r, g, and b values then store into array
		while(tok != NULL){
			cFlag++;
			if(cFlag == 1){
				r = atoi(tok);
			} else if (cFlag == 2) {
				g = atoi(tok);	
			}else{
				b = atoi(tok);
				//colors can be accessed via colors[j*width + i]
				t->colors[count] = (color){r,g,b};
				count++;
				cFlag = 0;
			}
			tok = strtok(NULL, " ");
		}
	}
	return t;
}
