#if !defined LINKED_LIST_H
#define LINKED_LIST_H

typedef struct LINKED_LIST_ENTRY {
	char * name;                           // name associated ot this entry
	char * value;                          // value associated to this name
	struct LINKED_LIST_ENTRY * next;       // pointer to the next entry in the list.
	struct LINKED_LIST_ENTRY * previous;   // pointer to the previous entry in the list.
} LINKED_LIST_ENTRY;

typedef struct LINKED_LIST {
	int nElements;                         // Number of elements stored in the list
	LINKED_LIST_ENTRY * head;              // Points to a dummy entry that simplifies the implementation.
	                                       // This entry is not used to stored data. It is only used to
	                                       // delimit the list.
} LINKED_LIST;                                

LINKED_LIST * llist_create();
void llist_print(LINKED_LIST * list);
int llist_add(LINKED_LIST * list, char * name, char * value);
char * llist_lookup(LINKED_LIST * list, char * name);
int llist_remove(LINKED_LIST * list, char * name);
int llist_get_ith(LINKED_LIST * list, int ith, char ** name, char ** value);
int llist_remove_ith(LINKED_LIST * list, int ith);
int llist_number_elements(LINKED_LIST * list);
int llist_save(LINKED_LIST * list, char * file_name);
int llist_read(LINKED_LIST * list, char * file_name);
void llist_sort(LINKED_LIST * list, int ascending);

int llist_remove_first(LINKED_LIST * list);
int llist_remove_last(LINKED_LIST * list);
int llist_insert_first(LINKED_LIST * list, char *name, char * value);
int llist_insert_last(LINKED_LIST * list, char *name, char * value);

#endif
