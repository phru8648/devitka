#include "list.h"
#include "edda.h"

#define CONFIG_FILE "devitka.yaml"

typedef struct config {

	list_t* eddas;
} config_t;

config_t* load_config(void);
void config_delete(config_t* config);

edda_t* config_get_edda(config_t* config, char* edda_name);
