
#include <stdio.h>
#include "stack.h"
#include <stdlib.h>

int top=0;
double stack[MAXSTACK];

void stack_clear() 
{
  top = 0;
}

double stack_pop()
{
	// Add implementation here
	double val = stack[top]; //get value from top
	top--; //move top back one
	return val;
}

void stack_push(double val)
{
	// Add implementation here
	stack[++top] = val; //move top and add value
}

void stack_print()
{
	if (top == 0) {
		printf("Stack:\n");
		printf("Stack is empty");
	}
	else {
		int x;
		printf("Stack:\n");
		for(x=0; x<top; x++) {
			printf("%d: %f\n", x, stack[x+1]);
		}
	}

}

int stack_top()
{
  return top;
}

int stack_max()
{
  return MAXSTACK;
}

int stack_is_empty()
{
  return top == 0;
}


