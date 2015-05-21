#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void toLower (char *c) {
	if (c >= 'A' && c <= 'Z') {
		c = (c - 'A') + 'a';
	}
}

int isPalindrome(char *str){
	int front = 0;
	int end = strlen(str)-1;
	while(front < end) {
		
		char c = str[front];
		while ( !( (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))) front++;
		c = str[end];
		while ( !( (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))) end++;

		if (toLower(&str[front]) == toLower(&str[end])) {
			front++;
			end++;
		}
		else {
			return 0;
		}
	}
	return 1;
}

int main( int argc, char *argv[] ){
	if( argc!=2 ){
		printf("Error\n");
		exit(4);
	}
	if( isPalindrome(argv[1]) ){
		printf("Yes\n");
	} else {
		printf("No\n");
	}
}
