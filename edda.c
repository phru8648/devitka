#include <stdlib.h>

#include "edda.h"

void edda_delete(edda_t* edda) {
	if (edda) {
		free(edda->name);
		free(edda->filename);
	}
}
