#ifndef STACK
#define STACK

#include "../Color/color.h"
typedef struct colorNode colorNode;
struct colorNode{
	color *c;
	colorNode *next;
};

typedef struct{
	colorNode *top;
	int count;
}colorStack ;

colorStack createColorStack();
color *peek(colorStack*);
color pop(colorStack*);
int push(colorStack*, color*);
#endif
