
#include <stdlib.h>
#include "mystring.h"

// Type "man string" to see what every function expects.

int mystrlen(char * s) {
	int i = 0;
	while (*s != '\0') {
		i++;
		s++;
	}
	return i;
}

char * mystrcpy(char * dest, char * src) {
	char*q = dest;
	char*p = src;

	while (*p) {
		*q = *p;
		*q++;
		*p++;
	}
	*q = '\0';

	return dest;
}

char * mystrcat(char * dest, char * src) {

	int i=0, j=0;

	while (*(dest+i) != '\0')	i++;

	while (*(src+j) != '\0') {
		*(dest+i+j) = *(src+j);
		j++;
	}

	*(dest+j+i) = '\0';
	return dest;

}

int mystrcmp(char * s1, char * s2) {

	int i;

	i = 0;

	while (*s1 != '\0') {
		if (*s2 == '\0') {
			return 1;
		}
		else if (*s2 > *s1) {
			return -1;
		}
		else if (*s2 < *s1) {
			return 1;
		}

		s1++;
		s2++;
	}

	if (*s2 != '\0') {
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

	char *d = malloc (mystrlen (s) + 1);
	if (d == NULL) return NULL;

	mystrcpy(d, s);
	return d;
}

char * mymemcpy(char * dest, char * src, int n)
{
	int i = 0;

	for (i = 0; i < n; i++) {
		*dest = *src;
		dest++;
		src++;
	}

	return NULL;
}

