
#include <stdlib.h>
#include "mystring.h"

// Type "man string" to see what every function expects.

int mystrlen(char * s) {
	int i = 0;
	while (s[i] != '\0') {
		i++;
	}
	return i;
}

char * mystrcpy(char * dest, char * src) {
	int i = 0;
	while (src[i] != '\0') {
		dest[i] = src[i];
		i++;
	}
	dest[i] = '\0';

	return dest;
}

char * mystrcat(char * dest, char * src) {

	int i, j;
	i = j = 0;

	while (dest[i] != '\0') {
		i++;
	}
	
	while (src[j] != '\0') {
		dest[i+j] = src[j];
		j++;
	}

	dest[i+j] = '\0';
	return dest;
}

int mystrcmp(char * s1, char * s2) {

	int i;

	i = 0;

	while (s1[i] != '\0') {
		if (s2[i] == '\0') {
			return 1;
		}
		else if (s2[i] > s1[i]) {
			return -1;
		}
		else if (s2[i] < s1[i]) {
			return 1;
		}

		i++;
	}

	if (s2[i] != '\0') {
		return -1;
	}

	return 0;
}

char * mystrstr(char * hay, char * needle) {
	while (*hay != '\0') {
		int i = 0;
		char * sub = needle; //pointer for needle

		while (*sub != '\0') { //as long as sub is not at end
			if (*sub == *hay) { //if pointer char == hay pointer char
				sub++;	//move forward 1
				hay++;
				i++; //add 1 to counter

				if (*sub == '\0') {  //if sub has reached end and all are same
					return hay - i; //return new position
				}
			}
			else {
				hay -= i;	//else 
				break;
			}
		}
		hay++; //move hay pointer 1 forward
	}
	return 0;
}

char * mystrdup(char * s) {

	int i, j;
	i = j = 0;

	while (s[i] != '\0') {
		i++;
	}

	char * x = malloc (i + 1);

	if (x != NULL) {
		while (s[j] != '\0') {
			x[j] = s[j];
			j++;
		}
		
		x[j] = '\0';
	}

	return x;
}

char * mymemcpy(char * dest, char * src, int n)
{
	int i;

	for (i = 0; i < n; i++) {
		dest[i] = src[i];
	}
	
	return NULL;
}

