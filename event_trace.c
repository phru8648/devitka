#include <stdio.h>
#include <stdlib.h>

#include "event_trace.h"

int search(struct event_trace* trace, int t) {

	if (!trace->count || t < trace->events[0].t) {
		return -1;
	}

	int l = 0; // l points to an event which is not later then event returned
	int r = trace->count; // right boundary is first invalid index


	while (r - l > 1) {

		int m = (l + r) / 2; // l < m < r

		if (trace->events[m].t <= t) {
			l = m;
		} else {
			r = m;
		}
	}

	return l;
}

struct event_trace* load_event_trace(const char* filename) {

	struct event_trace* trace = malloc(sizeof(struct event_trace));

	int size = 16;

	trace->events = (struct event*) malloc(sizeof(struct event) * size);

	int count = 0;

	FILE* f = fopen(filename, "rt");
	while (!feof(f)) {
		if (count == size) {
			size *= 2;
			trace->events = (struct event*) realloc(trace->events, sizeof(struct event) * size);
		}

		struct event* e = &trace->events[count++];
		fscanf(f, "%d %d\n", &e->t, &e->val);
	}
	trace->events = (struct event*) realloc(trace->events, sizeof(struct event) * count);
	trace->count = count;
	fclose(f);

	return trace;
}


void delete_event_trace(struct event_trace* trace) {

	free(trace->events);
	free(trace);
}
