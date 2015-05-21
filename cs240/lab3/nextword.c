
#include <stdio.h>
#include <stdlib.h>

//
// Separates the file into words
//

#define MAXWORD 200
#define IN 1
#define OUT 2

char word[MAXWORD];
int wordLength;
int STATE;

// It returns the next word from fd.
// If there are no more more words it returns NULL. 
char * nextword(FILE * fd) {
  	int c;
	wordLength = 0;
	STATE = OUT;
	while ((c = fgetc(fd)) != EOF) { //while not EOF read char
		while (c != EOF && c != ' ' && c != '\r' && c != '\t' && c != '\n') { //while not EOF and non space
			STATE = IN;
			word[wordLength++] = c; //add to word
			c = fgetc(fd);
		}
		if (c == EOF || c == ' ' || c == '\r' || c == '\t' || c == '\n') { //if equal to space
			if (STATE == IN) {
				word[wordLength] = '\0';
				return word;
			}
		}
	}
	return NULL;
/*
	STATE = OUT; //start in word	
	while ((c = fgetc(fd)) != EOF) {	// While it is not EOF read char
		while(c != EOF && c != ' ' && c != '\r' && c != '\t' && c!= '\n') {
		//	if (c != ' ' && c != '\r' && c != '\t' && c != '\n') { //if char
				word[wordLength++] = c; //add to word
				STATE = IN;
		//	}
		}
			if (c == ' ' || c == '\r' || c == '\t' || c == '\n') { //if equal to space
				if (STATE == IN) {
					return word;
				}
			}

		c = fgetc(fd);
		//}
	}	//
	// Return null if there are no more words
	return NULL;
*/
}

