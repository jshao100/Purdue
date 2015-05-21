
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Adds src at the end of dest. Return the value of dest.
char *mystrcat(char * dest, char * src) {
	char *p = dest;
	char *q = src;
	while (*p != '\0') p++;
	while (*q != '\0') {
		*p = *q;
		p++;
		q++;
	}
	*p = '\0';
  return dest;
}


// Return a pointer to the first occurrence of the substring needle in
// the string haystack. 
// Return NULL if the string is not found.
char * mystrstr(char * haystack, char * needle) {
	char * p = haystack;
	char * q = needle;
	char * last;

	while (*p != '\0') {
		last = p;
		while (*p == *q) {
			if (*(q+1) == '\0') return last;
			else {
				p++;
				q++;
			}
		}
		p = last;
		p++;
		q = needle;
	}
	return 0;
}


// Trims any spaces that are at the beginning or the end of s and returns s.
// Use the same memory passed in s. 
// Example:
// char s1[20];
// strcpy(s1,      hello    );
// strtrim(s1);
// printf(<s1=%s>\n, s1); // it will print <s1=hello>  without the spaces.
char * mystrtrim(char * s) {

	const char* leadingSpace = s;
	while(*leadingSpace != '\0' && isspace(*leadingSpace)) {
		++leadingSpace;
	}

	size_t len = strlen(leadingSpace)+1;

	memmove(s, leadingSpace, len);

	char* trailingSpace = s + len;

	while (s < trailingSpace && (isspace(*trailingSpace)) || (*trailingSpace == '\0')) {
		--trailingSpace;
	}

	*(trailingSpace+1) = '\0';

	return s;
}


// Returns a new string that will substitute the first occurrence of the substring from
//  to the substring to in the string src
// This call will allocate memory for the new string and it will return the new string.
// Example:
// char a[6];
// strcpy(a, apple);
// char * b = strreplace(a, pp, orange);
// printf(<b=%s>\n, b); // It will print <b=aorangele>  
char * mystrreplace( char * src, char * from, char * to)  {
	char *result;
	char *insert;
	char *tmp;
	int len_from;
	int len_to;
	int len_front;
	int count;

	if(!src)	return NULL;
	if(!from)	from = "";
	len_from = strlen(from);
	if(!to)		to = "";
	len_to = strlen(to);

	insert = src;
	for (count = 0; tmp = strstr(insert, from); ++count) {
		insert = tmp + len_from;
	}

	tmp = result = malloc(strlen(src) + (len_to - len_from) * count + 1);

	if(!result)		return NULL;

	while (count--) {
		insert = strstr(src, from);
		len_front = insert - src;
		tmp = strncpy(tmp, src, len_front) + len_front;
		tmp = strcpy(tmp, to) + len_to;
		src += len_front + len_from;
	}

	strcpy(tmp, src);
	return result;

}


