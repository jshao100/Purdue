
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char * mystrcat(char * dest, char * src);
char * mystrstr(char * haystack, char * needle);
char * mystrtrim(char * s);
char * mystrreplace( char * src, char * from, char * to);

void test1()
{
	char s[30];
  	strcpy(s, "CS240 ");
	mystrcat(s, "is a great class");
	printf("%s\n",s);
}

void test2()
{
    	char * s = "Hello world";
        char * sub = mystrstr("Hello world", "wo");
	printf("%s\n", sub);
	sub = mystrstr("Hello world", "He");
	printf("%s\n", sub);
	sub = mystrstr("Hello world", "ldf");
	printf("%s\n", sub?sub:"NULL");
	sub = mystrstr("Hello world", "aaa");
	printf("%s\n", sub?sub:"NULL");
}

void test3()
{
	char s1[20];
	strcpy(s1,"    hello   ");
	printf("Original: s1=<%s>\n", s1);
	mystrtrim(s1);
	printf("s1=<%s>\n", s1);

	strcpy(s1,"hello   ");
	printf("Original: s1=<%s>\n", s1);
	mystrtrim(s1);
	printf("s1=<%s>\n", s1);
	
	strcpy(s1,"    hello");
	printf("Original: s1=<%s>\n", s1);
	mystrtrim(s1);
	printf("s1=<%s>\n", s1);

	strcpy(s1,"hello");
	printf("Original: s1=<%s>\n", s1);
	mystrtrim(s1);
	printf("s1=<%s>\n", s1);
}

void test4() {
	char a[6];
	char * from = "pp";
	char * to = "orange";
	strcpy(a, "apple");
	printf("original=<%s>\n",a);
	printf("from=<%s>\n",from);
	printf("to=<%s>\n",to);
	char * b = mystrreplace(a, from, to);
	printf("replace=<b=%s>\n", b);


        char * s = "Dear friend, We thank you friend for your present.\n"
                   "We hope to see you friend in our next party.\n"
                   "Sincerely yours.\n";

        from = "friend";
        to = "George";
	printf("original=%s\n", s);
	printf("from=<%s>\n", from);
	printf("to=<%s>\n", to);
        char * result = mystrreplace(s, from, to);
	printf("result=<%s>\n", result);

	s = "aaa bbbbb a bbbbaa bbb vvv bb fff b";
        from = "bb";
        to = " xxx ";
	printf("original=%s\n", s);
	printf("from=<%s>\n", from);
	printf("to=<%s>\n", to);
        result = mystrreplace(s, from, to);
	printf("result=<%s>\n", result);
         
}

int
main(int argc, char ** argv) {

  	char * test;
  
  	if (argc <2) {
    		printf("Usage: test_mystring test1|test2|test3|test4|\n");
   		exit(1);
  	}
  
  	test = argv[1];
  	printf("Running %s\n", test);
  	if (strcmp(test, "test1")==0) {
    		test1();
  	}
  	else if (strcmp(test, "test2")==0) {
    		test2();
  	}
  	else if (strcmp(test, "test3")==0) {
    		test3();
  	}
  	else if (strcmp(test, "test4")==0) {
    		test4();
  	}
	else {
		printf("Wrong test");
	}
}
