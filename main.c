#include <stdio.h>

//#include "cursor.h"

#include "config.h"

int main(int argc, char* argv[]) {

	config_t* config = load_config();

	if (!config) {
		fprintf(stderr, "Error loading configuration file \"%s\"\n", CONFIG_FILE);
	}

	for (list_t* it = list_first(config->eddas); !list_eol(config->eddas, it); it = list_next(it)) {
		edda_t* edda = (edda_t*)list_element(it);
		printf("%s\n", edda->name);
	}

	config_delete(config);

//	struct event_trace* trace = load_event_trace("a.txt");
//	printf("trace event count = %d\n", trace->count);
//	delete_event_trace(trace);

}

