#include "event.h"

struct event_trace {

	int count;
	struct event* events;
};

// Return index of latest event with time equal or smaller than t. When there is no such event, return -1.
int search(struct event_trace* trace, int t);

struct event_trace* load_event_trace(const char* filename);
void delete_event_trace(struct event_trace* trace);
