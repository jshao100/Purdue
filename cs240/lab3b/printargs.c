
#include <stdio.h>

int main(int argc, char ** argv) {
	int i;

	printf("Printing arguments:\n");
	for (i=0; i < argc; i++) {
		printf("arg[%d]=%s\n", i,argv[i]);
	}
}

