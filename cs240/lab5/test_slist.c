
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "slist.h"

void test1()
{
	SLList list;
	sllist_init(&list);
	sllist_add_end(&list, 2);
	sllist_add_end(&list, 4);
	sllist_add_end(&list, 1);
	sllist_add_end(&list, 8);
	sllist_add_end(&list, 3);
	sllist_print(&list);
	sllist_add_end(&list, 2);
	sllist_add_end(&list, 4);
	sllist_add_end(&list, 1);
	sllist_add_end(&list, 8);
	sllist_add_end(&list, 3);
	sllist_print(&list);
}

void test2()
{
	SLList list;
	sllist_init(&list);
	sllist_add_end(&list, 2);
	sllist_add_end(&list, 4);
	sllist_add_end(&list, 1);
	sllist_add_end(&list, 8);
	sllist_add_end(&list, 3);
	sllist_add_end(&list, 2);
	sllist_add_end(&list, 4);
	sllist_add_end(&list, 1);
	sllist_add_end(&list, 8);
	sllist_add_end(&list, 3);
	sllist_print(&list);

	printf("remove 8\n");
	sllist_remove(&list, 8);
	sllist_print(&list);
	printf("remove 3\n");
	sllist_remove(&list, 3);
	sllist_print(&list);
	printf("remove 3\n");
	sllist_remove(&list, 3);
	sllist_print(&list);
	printf("remove 2\n");
	sllist_remove(&list, 2);
	sllist_print(&list);
	
}

void test3()
{
	SLList list;
	sllist_init(&list);
	sllist_add_end(&list, 2);
	sllist_add_end(&list, 4);
	sllist_add_end(&list, 1);
	sllist_add_end(&list, 8);
	sllist_add_end(&list, 3);
	sllist_add_end(&list, 2);
	sllist_add_end(&list, 4);
	sllist_add_end(&list, 1);
	sllist_add_end(&list, 8);
	sllist_add_end(&list, 3);
	sllist_print(&list);

	printf("==== remove interval 8<=x<=9 =====\n");
	sllist_remove_interval(&list, 8,9);
	sllist_print(&list);
	printf("\n==== remove interval 1<=x<=3 =====\n");
	sllist_remove_interval(&list, 1,3);
	sllist_print(&list);
}

void test4() {
	SLList list1;
	printf("======= List 1 =====\n");
	sllist_init(&list1);
	sllist_add_end(&list1, 2);
	sllist_add_end(&list1, 4);
	sllist_add_end(&list1, 1);
	sllist_add_end(&list1, 8);
	sllist_add_end(&list1, 2);
	sllist_add_end(&list1, 15);
	sllist_print(&list1);

	SLList list2;
	printf("======= List 2 =====\n");
	sllist_init(&list2);
	sllist_add_end(&list2, 3);
	sllist_add_end(&list2, 5);
	sllist_add_end(&list2, 7);
	sllist_add_end(&list2, 8);
	sllist_add_end(&list2, 1);
	sllist_add_end(&list2, 10);
	sllist_add_end(&list2, 11);
	sllist_add_end(&list2, 12);
	sllist_add_end(&list2, 13);
	sllist_add_end(&list2, 2);
	sllist_print(&list2);

	SLList * result = sllist_intersection(&list1, &list2);
	printf("====== Intersection ======\n");
	sllist_print(result);
}

int
main(int argc, char ** argv) {

  	char * test;
  
  	if (argc <2) {
    		printf("Usage: test_slist test1|test2|test3|test4|\n");
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
