#include <stdio.h>

//#include "cursor.h"

#include "config.h"

int main(int argc, char* argv[]) {

	if (!load_config()) {
		fprintf(stderr, "Error loading configuration file \"%s\"\n", CONFIG_FILE);
	}

//	struct event_trace* trace = load_event_trace("a.txt");
//	printf("trace event count = %d\n", trace->count);
//	delete_event_trace(trace);

}

