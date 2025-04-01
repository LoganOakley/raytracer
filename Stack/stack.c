#include "stack.h"
#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

colorStack createColorStack(){
	colorNode *bot = 0;
	return (colorStack){bot, 0};
}

color *peek(colorStack* stack){
	if(stack->count <= 0){ 
		return NULL;
	}
	return stack->top->c;
}

color pop(colorStack* stack){
	if(stack->count <= 0){
		printf("Pop from empty stackt");
		exit(-1);
	}

	color c = *stack->top->c;
	colorNode *next = stack->top->next;

	free(stack->top);
	stack->top = next;
	stack->count--;
	
	return c;
}

int push(colorStack* stack, color* c){
	colorNode *newNode = malloc(sizeof(colorNode));
	newNode->c=c;
	newNode->next=stack->top;
	stack->count++;
	stack->top = newNode;;
	return stack->count;
}

