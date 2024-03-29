#include <stdio.h>
#include <yaml.h>

#include "config.h"
#include "list.h"

#define EDDAS_KEY "eddas"
#define NAME_KEY "name"
#define FILENAME_KEY "filename"

struct context {

	yaml_parser_t parser;
	yaml_event_t event;

	config_t* config;
};

void log_yaml_parser_error(yaml_parser_t* parser) {

	fprintf(stderr, "Yaml parsing error:\"%s\"\n", parser->problem);
}

int config_init(config_t* config) {

	config->eddas = list_alloc();
	if (!config->eddas) {
		fprintf(stderr, "Cannot allocate list of eddas for config\n");
		return 0;
	}
	return 1;
}

void config_delete(config_t* config) {

	list_t* it = list_first(config->eddas); 
	while (!list_eol(config->eddas, it)) {

		edda_t* edda = (edda_t*)list_element(it);
		edda_delete(edda);
		it = list_next(it);
	}

	list_delete(config->eddas);
}

int context_init(struct context* ctx, FILE* f, config_t* config) {

	ctx->config = config;

	if (!yaml_parser_initialize(&ctx->parser)) {
		fprintf(stderr, "Yaml parser initialization error:\"%s\"\n", ctx->parser.problem);
		return 0;
	}

	yaml_parser_set_input_file(&ctx->parser, f);

	// this is usually called via parse_event functiuon below, but parse_event deletes event from last parsing, 
	// which is not initialized yet
	if (!yaml_parser_parse(&ctx->parser, &ctx->event)) {
		log_yaml_parser_error(&ctx->parser);
		yaml_parser_delete(&ctx->parser);
		yaml_event_delete(&ctx->event);
		return 0;
	}

	if (ctx->event.type != YAML_STREAM_START_EVENT) {
		fprintf(stderr, "Event YAML_STREAM_START_EVENT expected, but event type %d found\n", ctx->event.type);
		yaml_parser_delete(&ctx->parser);
		yaml_event_delete(&ctx->event);
		return 0;
	}

	return 1;
}

int parse_event(struct context* ctx) {

	yaml_event_delete(&ctx->event);
	if (!yaml_parser_parse(&ctx->parser, &ctx->event)) {
		log_yaml_parser_error(&ctx->parser);
		return 0;
	}

	return 1;
}

int parse_event_type(struct context* ctx, yaml_event_type_t event_type) {

	if (!parse_event(ctx)) {
		return 0;
	}

	if (ctx->event.type != event_type) {
		fprintf(stderr, "Event type %d expected, but event type %d found\n", event_type, ctx->event.type);
		return 0;
	}

	return 1;
}

int parse_scalar(struct context* ctx) {

	if (!parse_event(ctx)) {
		return 0;
	}

	if (ctx->event.type != YAML_SCALAR_EVENT) {
		fprintf(stderr, "Scalar expected, but event type %d found\n", ctx->event.type);
		return 0;
	}

	return 1;
}

int parse_edda(struct context* ctx) {

	if (!parse_event(ctx)) {
		return 0;
	}

	edda_t* edda = (edda_t*)calloc(1, sizeof(edda_t));
	if (!edda) {
		fprintf(stderr, "Cannot allocate edda\n");
		return 0;
	}

	while (ctx->event.type != YAML_MAPPING_END_EVENT) {

		char** value_field;

		if (ctx->event.type == YAML_SCALAR_EVENT) {
			char* key = ctx->event.data.scalar.value;
			if (!strcmp(key, NAME_KEY)) {
				value_field = &edda->name;
				if (edda->name) {
					fprintf(stderr, "Duplicate occurence of `name` property\n");
					goto error;

				}
			} else if (!strcmp(key, FILENAME_KEY)) {
				if (edda->filename) {
					fprintf(stderr, "Duplicate occurence of `filename` property\n");
					goto error;

				}
				value_field = &edda->filename;
			} else {
				fprintf(stderr, "Unexpected edda attribute \"%s\"\n", key);
				goto error;
			}

		} else {
			fprintf(stderr, "Key expected, but event type %d found\n", ctx->event.type);
			goto error;
		}
		
		if (!parse_event(ctx)) {
			goto error;
		}

		if (ctx->event.type == YAML_SCALAR_EVENT) {
			char* value = ctx->event.data.scalar.value;
			*value_field = strdup(value);
			if (*value_field == NULL) {
				fprintf(stderr, "Unable to allocate memory when parsing config\n");
				goto error;
			}
		} else {
			fprintf(stderr, "Value expected, but event type %d found\n", ctx->event.type);
			goto error;
		}

		if (!parse_event(ctx)) {
			goto error;
		}
	}

	if (!edda->name) {
		fprintf(stderr, "Edda name not found \n");
		goto error;
	}

	if (!edda->filename) {
		fprintf(stderr, "Edda filename not found for edda \"%s\"\n", edda->name);
		goto error;
	}

	edda_t* existing_edda = config_get_edda(ctx->config, edda->name);
	if (existing_edda) {
		fprintf(stderr, "Edda name `%s` is declared more than once\n", edda->name);
		goto error;
	}


	printf("Adding edda `%s` to config (filename `%s`)\n", edda->name, edda->filename);
	list_append(ctx->config->eddas, edda);

	return 1;

error:
	edda_delete(edda);
	return 0;
}

int parse_eddas(struct context* ctx) {

	printf("Parsing eddas\n");

	if (!parse_event_type(ctx, YAML_SEQUENCE_START_EVENT)) {
		return 0;
	}

	if (!parse_event(ctx)) {
		return 0;
	}

	while (ctx->event.type != YAML_SEQUENCE_END_EVENT) {

		if (ctx->event.type != YAML_MAPPING_START_EVENT) {
			fprintf(stderr, "Mapping start expected\n");
			return 0;
		}

		if (!parse_edda(ctx)) {
			fprintf(stderr, "Error parsing edda\n");
			return 0;
		}

		if (!parse_event(ctx)) {
			return 0;
		}
	}

	return 1;
}

int parse_main_mapping(struct context* ctx) {

	if (!parse_event_type(ctx, YAML_MAPPING_START_EVENT)) {
		return 0;
	}

	if (!parse_event(ctx)) {
		return 0;
	}

	while (ctx->event.type != YAML_MAPPING_END_EVENT) {

		if (ctx->event.type == YAML_SCALAR_EVENT) {
			if (!strcmp(EDDAS_KEY, ctx->event.data.scalar.value)) {
				if (!parse_eddas(ctx)) {
					return 0;
				}
			} else {
				fprintf(stderr, "Unexpected key \"%s\" found\n", ctx->event.data.scalar.value);
				return 0;
			}
		} else {
			fprintf(stderr, "Unexpected event type %d found\n", ctx->event.type);
			return 0;
		}

		if (!parse_event(ctx)) {
			return 0;
		}
	}

	return 1;
}

int parse_document(struct context* ctx) {

	if (!parse_event_type(ctx, YAML_DOCUMENT_START_EVENT)) {
		return 0;
	}

	if (!parse_main_mapping(ctx)) {
		return 0;
	}

	if (!parse_event_type(ctx, YAML_DOCUMENT_END_EVENT)) {
		return 0;
	}

	return 1;
}

int parse_file(struct context* ctx) {

	// YAML_STREAM_START_EVENT is already parsed when context is initialized

	if (!parse_document(ctx)) {
		fprintf(stderr, "Error parsing yaml document\n");
		return 0;
	}

	if (!parse_event_type(ctx, YAML_STREAM_END_EVENT)) {
		return 0;
	}

	return 1;
}

config_t* load_config(void) {

	struct context ctx;

	FILE* f = fopen(CONFIG_FILE, "rt");
	if (!f) {
		fprintf(stderr, "Error reading configuration file \"%s\"\n", CONFIG_FILE);
		return NULL;
	}

	config_t* config = (config_t*)malloc(sizeof(config_t));
	if (!config_init(config)) {
		fprintf(stderr, "Error initializing config\n");
		free(config);
		return NULL;
	}

	if (!context_init(&ctx, f, config)) {
		config_delete(config);
		free(config);
		fprintf(stderr, "Error initializing parser context\n");
		return NULL;
	}

	int parse_result = parse_file(&ctx);

	yaml_event_delete(&ctx.event);
	yaml_parser_delete(&ctx.parser);
	fclose(f);

	if (!parse_result) {
		fprintf(stderr, "Error parsing configuration file \"%s\"\n", CONFIG_FILE);
		config_delete(config);
		free(config);
		return NULL;
	}

	return config;
}

edda_t* config_get_edda(config_t* config, char* edda_name) {

	list_t* it = list_first(config->eddas);
	while (!list_eol(config->eddas, it)) {
		edda_t* edda = (edda_t*)list_element(it);
		if (!strcmp(edda->name, edda_name)) {
			return edda;
		}
		it = list_next(it);
	}

	return NULL;
}
