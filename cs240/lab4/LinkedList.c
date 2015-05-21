
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "LinkedList.h"

//
// Initialize a linked list
//
void llist_init(LinkedList * list)
{
	list->head = NULL;
}

//
// It prints the elements in the list in the form:
// 4, 6, 2, 3, 8,7
//
void llist_print(LinkedList * list) {
	
	ListNode * e;

	if (list->head == NULL) {
		printf("{EMPTY}\n");
		return;
	}

	printf("{");

	e = list->head;
	while (e != NULL) {
		printf("%d", e->value);
		e = e->next;
		if (e!=NULL) {
			printf(", ");
		}
	}
	printf("}\n");
}

//
// Appends a new node with this value at the beginning of the list
//
void llist_add(LinkedList * list, int value) {
	// Create new node
	ListNode * n = (ListNode *) malloc(sizeof(ListNode));
	n->value = value;
	
	// Add at the beginning of the list
	n->next = list->head;
	list->head = n;
}

//
// Returns true if the value exists in the list.
//
int llist_exists(LinkedList * list, int value) {
	ListNode * e;

	if (list->head == NULL) {
		return 0;
	}
	
	e = list->head;
	while (e != NULL) {
		if (e->value == value) {
			return 1;
		}
		e = e->next;
	}
	return 0;
}

//
// It removes the entry with that value in the list.
//
int llist_remove(LinkedList * list, int value) {
	ListNode * e;
	e = list->head;

	ListNode * d = NULL;
	
	while (e != NULL) {
		if (e->value == value) {
			break;
		}
		d = e;
		e = e->next;
	}
	if (e == NULL) {
		return 0;
	}
	if (d == NULL) {
		list->head = e->next;
	}
	else {
		d->next = e->next;
	}

	return 1;
}

//
// It stores in *value the value that correspond to the ith entry.
// It returns 1 if success or 0 if there is no ith entry.
//
int llist_get_ith(LinkedList * list, int ith, int * value) {
	ListNode * e;

	e = list->head;
	int i = 0;

	while (e != NULL) {
		if (i == ith) {
			*value = e->value;
			return 1;
		}
		
		i++;
		e = e->next;
	}
	
	
	return 0;
}

//
// It removes the ith entry from the list.
// It returns 1 if success or 0 if there is no ith entry.
//
int llist_remove_ith(LinkedList * list, int ith) {
	ListNode * e;
	e = list->head;

	ListNode * d = NULL;
	int i = 0;

	while (e != NULL) {
		if (i == ith) {
			break;
		}
		i++;
		d = e;
		e = e->next;
	}

	if (e == NULL) return 0;
	if (d == NULL) list->head = e->next;
	else	d->next = e->next;
	
	return 1;
}

//
// It returns the number of elements in the list.
//
int llist_number_elements(LinkedList * list) {
	ListNode * e;

	e = list->head;
	int i = 0;

	while (e != NULL) {
		i++;
		e = e->next;
	}
	
	return i;
}


//
// It saves the list in a file called file_name. The format of the
// file is as follows:
//
// value1\n
// value2\n
// ...
//
int llist_save(LinkedList * list, char * file_name) {
	ListNode * e;
	e = list->head;

	FILE * f = fopen(file_name, "w");

	if (f == NULL) {
		return 0;
	}

	while (e != NULL) {
		fprintf(f, "%d\n", e->value);
		e = e->next;
	}

	fclose(f);

	return 0;
}

//
// It reads the list from the file_name indicated. If the list already has entries, 
// it will clear the entries.
//
int llist_read(LinkedList * list, char * file_name) {
	
	FILE * f = fopen(file_name, "r");
	char * line = NULL;
	ssize_t len = 0;
	ssize_t read;

	if (f == NULL)		return 0;

	while ((read = getline(&line, &len, f)) != -1) {
		//printf("%s",line);
		llist_add(list, atoi(line));
	}

//	llist_print(list);

	fclose(f);
	return 1;
}


//
// It sorts the list. The parameter ascending determines if the
// order si ascending (1) or descending(0).
//
void llist_sort(LinkedList * list, int ascending) {

	int swapped, i;

	ListNode * e;
	ListNode * d = NULL;

	if (e == NULL) ;
	do {
		swapped = 0;
		e = list->head;
		while (e->next != d) {

			if (ascending == 1) {
				if (e->value > e->next->value) {
				
					int tmp = e->value;
					e->value = e->next->value;
					e->next->value = tmp;
				
					swapped = 1;
				}
				e = e->next;
			}

			if (ascending == 0) {
				if (e->value < e->next->value) {
					int tmp = e->value;
					e->value = e->next->value;
					e->next->value = tmp;

					swapped = 1;
				}
				e = e->next;


			}

		}
		d = e;
	}
	while (swapped);

}


//
// It removes the first entry in the list and puts value in *value.
// It also frees memory allocated for the node
//
int llist_remove_first(LinkedList * list, int * value) {
	ListNode * e;
	e = list->head;

	if (e == NULL) {
		return 0;
	}

	*value = e->value;
	free(e);
	list->head = e->next;

	return 1;
}

//
// It removes the last entry in the list and puts value in *value/
// It also frees memory allocated for node.
//
int llist_remove_last(LinkedList * list, int *value) {
	ListNode * e;
	e = list->head;

	ListNode * d;
	d = e;

	if (e == NULL) 	return 0;

	while (e != NULL && e->next != NULL) {
		d = e;
		e = e->next;
	}

	if (d == e) {
		*value = e->value;
		free(e);
		list->head = NULL;
	}
	else {
		*value = d->next->value;
		free(d->next);
		d->next = NULL;
	}

	return 1;
}

//
// Insert a value at the beginning of the list.
// There is no check if the value exists. The entry is added
// at the beginning of the list.
//
void llist_insert_first(LinkedList * list, int value) {
	ListNode * n = (ListNode *) malloc(sizeof(ListNode));
	n->value = value;

	n->next = list->head;
	list->head = n;
}

//
// Insert a value at the end of the list.
// There is no check if the name already exists. The entry is added
// at the end of the list.
//
void llist_insert_last(LinkedList * list, int value) {
	//printf("insert last");
	
	ListNode * n = (ListNode *) malloc(sizeof(ListNode));
	n->value = value;

	ListNode * e;
	e = list->head;

	int i = llist_number_elements(list);
	if (i == 0) {
		llist_insert_first(list, value);
		return;
	}

	while (e->next != NULL) {
		e = e->next;
	}
	e->next = n;
	n->next = NULL;
}

//
// Clear all elements in the list and free the nodes
//
void llist_clear(LinkedList *list)
{
	ListNode * e;
	e = list->head;
	ListNode * d;

	while (e != NULL) {
		d = e->next;
		free(e);
		e = d;
	}
	list->head = NULL;
}
