#ifndef _SCONF_H_
#define _SCONF_H_

#define DEBUG 1

/* Return codes */
#define SCONF_OKAY        0
#define SCONF_INPUT_EMPTY 1
#define SCONF_OTHER_ERROR 2

#define SCONF_MAX_KEY_LEN   128
#define SCONF_MAX_VALUE_LEN 512

/*
 *
 * Typically, type-safety when working with arbitrary data is provided
 *   through reflection, particularly as it enables a function to take
 *   arbitrary data and identify its fields at runtime.
 * C does not have reflection.
 *
 * The solution then, is to use a macro that creates a function that takes
 *   a struct; and creates one parsing function for every struct. One could use
 *   overloading, a feature of languages that enables multiple functions to be
 *   defined with the same name, but different signatures - there may be an
 *   arbitrary quantity of functions called sconf_load_string, as long as
 *   the signatures (for example, the type of the struct the function takes)
 *   are different:
 *    int sconf_load_string(struct config_struct,  int flags, char *data);
 *    int sconf_load_string(struct another_struct, int flags, char *data);
 * C does not have overloading.
 *
 * The solution then, is to use a macro that creates a function that takes
 *   a struct; and creates a parsing function with a unique name. Of course,
 *   this would make the API unwieldy and frightening - relying on guessing
 *   what function name the macro spits out. It also would not be possible to
 *   create an sconf_load_string function that simply wraps all of these
 *   internal functions, lacking unique runtime identifiers and C's inability
 *   to take arbitrarily typed arguments (or, there may be another solution
 *   here using varargs hackery).
 *   As such, sconf_load_string is not actually a function, but a macro
 *   which replaces itself with an invocation to that internal parsing
 *   function.
 *     sconf_load_string(config_example, config, 0, config_string, ret_val);
 *   actually ends up a lot like:
 *     do {
 *       ret_val =  __SCONF_INTERNAL_config_example(config, 0, config_string);
 *     } while (0);
 *
 *
 * Nothing past this point is relevant to the publicly facing API.
 */

#if DEBUG
// Feel good?
#define DEBUG_INC(x) do { x } while (0);
#else
#define DEBUG_INC(x) do { } while (0);
#endif

typedef struct {
    char key[SCONF_MAX_KEY_LEN];
    char value[SCONF_MAX_VALUE_LEN];
} kv_pair;

int _sconf_parse_text(kv_pair *, char *, int *);

#define _MACRO_CONCAT(x, y) x##y
#define MACRO_CONCAT(x, y) _MACRO_CONCAT(x, y)

#define VA_ARGS_EMPTY(...) (sizeof((char[]){#__VA_ARGS__}) == 1)

/*#define SCONF_SET_STRUCT_VAL_(conf_struct_instance_name, kv_pairs, total_count, set, ...)*/ \
#define SCONF_SET_STRUCT_VAL_(conf_struct_instance_name, kv_pairs, total_count, set) \
	do { \
	    for (int i = 0; i < total_count; i++) { \
	   		if (strcmp(kv_pairs[i].key, #set) == 0) { \
	   			strcpy(conf_struct_instance_name->set, kv_pairs[i].value); \
	   		} \
	    } \
	    /*if (!VA_ARGS_EMPTY(__VA_ARGS__)) { \
			SCONF_SET_STRUCT_VAL(conf_struct_instance_name, kv_pairs, total_count, __VA_ARGS__) \
	    }*/ \
	} while (0)

#define SCONF_AWARE(conf_struct_name, ...) \
	/* Replace values in the target configuration struct with the values stored in the key-value \
	 * pairs */ \
    void \
    MACRO_CONCAT(__SCONF_INTERNAL_SET_VALS_, conf_struct_name) ( \
    	kv_pair *kv_pairs, \
        int total_count, \
		struct conf_struct_name *conf_struct \
    ) { \
        char *keys = strdup(#__VA_ARGS__); \
        char *key; \
		int kv_idx = 0; \
        while ((key = strsep(&keys, ",")) != NULL) { \
			DEBUG_INC(printf("strsep: %s\n", key);); \
			\
			SCONF_SET_STRUCT_VAL_(conf_struct, kv_pairs, total_count, ##key); \
        } \
    } \
\
	/* Allocate key-value pairs and parse the configuration file (as a string) */ \
	int \
	MACRO_CONCAT(__SCONF_INTERNAL_, conf_struct_name) ( \
		struct conf_struct_name *conf_struct, \
		int config_options, \
		char *data \
	) { \
		kv_pair *kv_pairs = malloc(sizeof(kv_pair) * 4); \
   	 	int total_count = 0; \
		int ret_code = _sconf_parse_text(kv_pairs, data, &total_count); \
		MACRO_CONCAT(__SCONF_INTERNAL_SET_VALS_, conf_struct_name) \
			(kv_pairs, total_count, conf_struct); \
		free(kv_pairs); \
		return ret_code; \
	}

#define sconf_load_string(conf_struct_name, conf_struct, mode, string, ret) \
	do { \
		ret = MACRO_CONCAT(__SCONF_INTERNAL_, conf_struct_name) (conf_struct, mode, string); \
	} while (0)

#endif

int
_sconf_parse_text(kv_pair *kv_pairs, char *data, int *total_count)
{
    strcpy(kv_pairs[0].key, "hostname");
    strcpy(kv_pairs[0].value, "differentHostname");

	kv_pairs = realloc(kv_pairs, 2 * sizeof(kv_pair));

	strcpy(kv_pairs[1].key, "public");
	strcpy(kv_pairs[1].value, "1");
    *total_count = 2;
	return 0;

	if (data == NULL)
    	return SCONF_INPUT_EMPTY;

	char current_key[SCONF_MAX_KEY_LEN];
	char current_value[SCONF_MAX_VALUE_LEN];
	int current_key_length   = 0;
	int current_value_length = 0;
	return 1;
}

/*
int
_sconf_parse_text(kv_pair **kv_pairs, char *data)
{
    if (data == NULL)
		return SCONF_INPUT_EMPTY;

	char current_key[SCONF_MAX_KEY_LEN];
	char current_value[SCONF_MAX_VALUE_LEN];
	int current_key_length = 0;
	int current_value_length = 0;

	int kv_encountered = 0;
	int kv_allocated   = 1;

	int context = _SCONF_PCTX_INIT;
	char *ch = data;
	while (*ch != '\0')
	{
    	printf("%c", *ch);

		switch (*ch) {
		case ' ':
		case '\r':
		case '\n':
			if (context == _SCONF_PCTX_RWAITVALUE) {
    			continue;
			} else if (context == _SCONF_PCTX_RKEY) {
    			context = _SCONF_PCTX_RVALUE;
				continue;
			} else if (context == _SCONF_PCTX_RVALUE) {
    			if (kv_encountered <= kv_allocated)
					kv_pairs = (kv_pair **)realloc(kv_pairs, (kv_allocated * 2) * sizeof(kv_pair));
    				kv_allocated *= 2;

        		current_key[current_key_length] = '\0';
   				current_value[current_value_length] = '\0';

				strcpy(kv_pairs[kv_encountered]->key, current_key);
				strcpy(kv_pairs[kv_encountered]->value, current_value);

        		kv_encountered++;
    			context = _SCONF_PCTX_WAIT;
    			continue;
			}
			DEBUG_INC({
				printf("bad state. dumping.\n");
				printf("context: %d\n", context);
				printf("current key (length): %s (%d)\n", current_key, current_key_length);
				printf("current value (length): %s (%d)\n", current_value, current_value_length);
				printf("exiting.\n");
			})
			return SCONF_OTHER_ERROR;
		case '(':
		case ')':
			continue;
		// Other char encountered
		default:
    		if (context == _SCONF_PCTX_RKEY ||
    		    context == _SCONF_PCTX_WAIT) {
        		current_key[current_key_length] = *ch;
        		current_key_length++;
    		} else if (context == _SCONF_PCTX_RVALUE) {
        		current_value[current_value_length] = *ch;
        		current_value_length++;
    		}
    	}

    	ch++;
	}

	DEBUG_INC({
		printf("parsing done, dumping %d values\n", kv_encountered);
		for (int i = 0; i < kv_encountered; i++) {
			printf("key: %s\n", kv_pairs[i]->key);
			printf("value: %s\n", kv_pairs[i]->value);
		}
	})

	return SCONF_OKAY;
}*/
