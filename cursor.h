#include "event_trace.h"

typedef struct cursor {

	struct event_trace* trace;
	int position;
} cursor_t;

cursor_t* open_cursor(struct event_trace* trace);

void close_cursor(cursor_t* cursor);

// Seek cursor to time t or to first event before t. When there is no such
// event, special position ANCIENT should be set into cursor.
void seek(cursor_t* cursor, int t);

// Get value.
int get(cursor_t* cursor);

// Return current value and move cursor if possible
int next(cursor_t* cursor);

// Return current value and move cursor if possible
int prev(cursor_t* cursor);

int ancient(cursor_t* cursor);

int last(cursor_t* cursor);
