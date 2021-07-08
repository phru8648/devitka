#include <stdlib.h>
#include <limits.h>

#include "cursor.h"

// When trace is modified, only cvalid operation on cursor is close_cursor().
cursor_t* open_cursor(struct event_trace* trace) {

	cursor_t* c = malloc(sizeof(cursor_t));
	c->position = -1;
	c->trace = trace;
	return c;
}

void close_cursor(cursor_t* cursor) {
	free(cursor);
}

// Seek cursor to time t or to first event before t. When there is no such
// event, special position ANCIENT should be set into cursor.
void seek(cursor_t* cursor, int t) {

	int pos = search(cursor->trace, t);
	cursor->position = pos;
}

// Get value
int get(cursor_t* cursor) {
	if (cursor->position == -1) {
		return INT_MIN;
	}
	if (cursor->position == cursor->trace->count) {
		return INT_MAX;
	}
	return cursor->trace->events[cursor->position].val;
}

// Return current value and move cursor if possible
int next(cursor_t* cursor) {

	if (cursor->position == cursor->trace->count) {
		return INT_MAX;
	}

	int value = INT_MIN;
	if (cursor->position >= 0) {
		value = cursor->trace->events[cursor->position].val;
	}

	cursor->position++;

	return value;
}

// Return current value and move cursor if possible
int prev(cursor_t* cursor) {

	if (cursor->position == -1) {
		return INT_MIN;
	}

	int value = INT_MAX;
	if (cursor->position < cursor->trace->count) {
		value = cursor->trace->events[cursor->position].val;
	}

	cursor->position--;

	return value;
}

int ancient(cursor_t* cursor) {
	return cursor->position == -1;
}

int last(cursor_t* cursor) {
	return cursor->position == cursor->trace->count;
}

