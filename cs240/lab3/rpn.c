
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <math.h>

#include "rpn.h"
#include "nextword.h"
#include "stack.h"

double rpn_eval(char * fileName, double x) {

	double a, b, c;
	FILE * fd;

	int operands = 0;
	int numbers = 0;

	fd = fopen(fileName, "r");

	char * word;

	while(word = nextword(fd)) {
		//printf("%s\n", word);
		//stack_print();
		if (strcmp("+",word) == 0) {
			a = stack_pop();

			if (stack_is_empty()) {
				printf("Stack underflow\n");
				exit(1);
			}

			b = stack_pop();
			
			c = b+a;
			stack_push(c);
			operands++;
		}
		else if (strcmp("-",word) == 0) {
			a = stack_pop();
			
			if (stack_is_empty()) {
			    printf("Stack underflow\n");
				exit(1);
		 	}

			b = stack_pop();
			c = b-a;
			stack_push(c);
			operands++;
		}
		else if (strcmp("*",word) == 0) {
			a = stack_pop();
			
			if (stack_is_empty()) {
			    printf("Stack underflow\n");
				exit(1);
			}
			
			b = stack_pop();
			c = b*a;
			stack_push(c);
			operands++;
		}
		else if (strcmp("/",word) == 0) {
			a = stack_pop();
			
			if (stack_is_empty()) {
			    printf("Stack underflow\n");
			    exit(1);
			}
			
			b = stack_pop();
			c = b/a;
			stack_push(c);
			operands++;
		}
		else if (strcmp("sin", word) == 0) {
			
			if (stack_is_empty()) {
			    printf("Stack underflow\n");
				exit(1);
			}
			
			a = stack_pop();
			c = sin(a);
			stack_push(c);
			operands++;
		}
		else if (strcmp("cos",word) == 0) {
			
			if (stack_is_empty()) {
			    printf("Stack underflow\n");
			    exit(1);
			}
			
			a = stack_pop();
			c = cos(a);
			stack_push(c);
			operands++;
		}
		else if (strcmp("pow",word) == 0) {
			a = stack_pop();
			
			if(stack_is_empty()) {
				printf("Stack underflow\n");
				exit(1);
			}

			b = stack_pop();
			c = pow(b,a);
			stack_push(c);
			operands++;
		}
		else if (strcmp("log",word) == 0) {
			
			if(stack_is_empty()) {
				printf("Stack underflow\n");
				exit(1);
			}

			a = stack_pop();
			c = log(a);
			stack_push(c);
			operands++;
		}
		else if (strcmp("exp",word) == 0) {
			
			if(stack_is_empty()) {
				printf("Stack underflow\n");
				exit(1);
			}
			
			a = stack_pop();
			c = exp(a);
			stack_push(c);
			operands++;
		}

		else if(strcmp("x",word) == 0) {
			//printf("%f",x);
			stack_push(x);
		}
		
		else { //if number	
			c = atof(word);
			stack_push(c);
			numbers++;
		}
	}

	c = stack_pop();

	if(stack_is_empty()) {
		return c;
	}
	else {
		printf("Elements remain in the stack\n");
		exit(1);
	}
}

