
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void mymemdump(FILE * fd, char * p , int len) {
    // Add your code here.
    // You may see p as an array.
    // p[0] will return the element 0 
    // p[1] will return the element 1 and so on

	int i,j;

	for (i = 0; i < len; i = i+16) {
		fprintf(fd, "0x%016lX: ", (unsigned long) &p[i]);
		for (j = 0; j < 16; j++) {
			int c = p[i+j]&0xFF;
			
			if (j+i < len) {
				fprintf(fd, "%02X ", c);
			}
			else {
				fprintf(fd, "   ");
			}
			
			//fprintf(fd, "%02X ", c);
		}
		fprintf(fd, " ");
		for (j = 0; j < 16 && j+i < len; j++) {
			int c = p[i+j]&0xFF;
			fprintf(fd, "%c", (c>=32 && c<=127)?c:'.');
		}
		fprintf(fd, "\n");
	}
}

