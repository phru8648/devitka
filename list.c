#include <stdlib.h>

#include "list.h"

struct item {

	struct item* prev;
	struct item* next;

	void* element;

};


list_t* list_alloc(void) {
	list_t* list = (list_t*)malloc(sizeof(list_t));
	list->prev = list;
	list->next = list;
	return list;
}

void list_delete(list_t* list) {

	list_t* x = list->next;
	while (x != list) {

		x = x->next;
		free(x->prev);
	}

	free(list);
}

void list_append(list_t* list, void* element) {

	list_t* x = (list_t*) malloc(sizeof(list_t));
	x->next = list;
	x->prev = list->prev;
	list->prev = x;
	x->prev->next = x;

	x->element = element;
}

// return nonzero when this item is eol record
int list_eol(list_t* list, list_t* it) {
	return list == it;
}

// for empty list, this is eol record
list_t* list_first(list_t* list) {
	return list->next;
}

list_t* list_next(list_t* it) {
	return it->next;
}

void* list_element(list_t* it) {
	return it->element;
}
