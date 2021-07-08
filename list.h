typedef struct item list_t;

list_t* list_alloc(void);

void list_delete(list_t* list);

void list_append(list_t* list, void* element);

// return nonzero when this item is eol record
int list_eol(list_t* list, list_t* it);

// for empty list, this is eol record
list_t* list_first(list_t* list);

list_t* list_next(list_t* it);

void* list_element(list_t* it);
