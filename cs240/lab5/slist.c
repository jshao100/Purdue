
#include <stdio.h>
#include <stdlib.h>

#include "slist.h"

void
sllist_init(SLList * list)
{
	list->head = NULL;
}


void sllist_add(SLList *list, int value) {
	SLEntry *e = list->head;
	e = (SLEntry*)malloc(sizeof(SLEntry));
	e->value = value;
	e->next = list->head;
	list->head = e;
}

// Add int value to the end of list. The values may be repeated.
void
sllist_add_end( SLList *list, int value )  {
	SLEntry *e = list->head;
	SLEntry *last = NULL;

	while (e!=NULL) {
		last = e;
		e = e->next;
	}

	SLEntry *new = (SLEntry*)malloc(sizeof(SLEntry));
	new->value = value;
	new->next= NULL;

	if (last == NULL) {
		list->head = new;
	}
	else {
		last->next = new;
	}
}


// Remove first occurrence of value in the list. It returns 1 if value is found or 0 otherwise.
int sllist_remove(SLList *list, int value) {
	SLEntry *e = list->head;
	SLEntry *last = NULL;

	while (e!=NULL) {
		if(value == e->value) {
			if(last==NULL) {
				list->head=e->next;
			}
			else {
				last->next=e->next;
			}
			return 1;
		}
		last = e;
		e=e->next;
	}
	return 0;
}


// Removes from the list and frees the nodes of all the items that are min <= value <= max
void sllist_remove_interval(SLList *list, int min, int max) {
	SLEntry *e = list->head;
	SLEntry *last = NULL;

	int i = min;
	while (i<=max) {
		int rem=1;
		while(rem) {
			rem = sllist_remove(list,i);
		}
		i++;
	}

}


// It allocates and returns a new list with the intersection of the two lists, that is the 
// list of all the items that are common to the two lists. The items in the intersection are
// unique. 
SLList * sllist_intersection(SLList *a, SLList *b) {
	//SLList list;
	SLList *list = (SLList *)malloc(sizeof(SLList));
	sllist_init(list); //create return list

	SLEntry *aa = a->head;
	SLEntry *bb = b->head;
	
	while (aa != NULL) {
		//printf("a: %d\n",aa->value);
		bb = b->head;
		while (bb != NULL) {
			//printf("b: %d\n",bb->value);
			if (aa->value == bb->value) {
				int value = aa->value;

				sllist_add(list, value);

				sllist_remove_interval(a, value, value);
				sllist_remove_interval(b, value, value);
				break;
			}
			bb = bb->next;
		}
		aa = aa->next;
	}
	
	return list;
}

void
sllist_print(SLList *list)
{
	// Move to the end
	SLEntry * e = list->head;

	printf("--- List ---\n");
	while (e!= NULL) {
		printf("val=%d\n", e->value);
		e = e->next;
	}
}

