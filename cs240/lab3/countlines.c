
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char ** argv) {
	int c;
	int lines=0;


	//get file
	char * fileName = argv[1];
	FILE * fd = fopen(fileName, "r");

 	// Add your implementation here
	while ((c = fgetc(fd)) != EOF) {
		if (c == '\n') lines++;
	}
	
	printf("Program that counts lines.\nTotal lines: %d\n", lines);

	exit(0);
}
