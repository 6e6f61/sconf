#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "../sconf.h"

struct config_example {
	char hostname[256];
	int public;
};

struct file_example {
	int debug_mode;
};

SCONF_AWARE(config_example, hostname, public)
//SCONF_AWARE(file_example)

// Example of using sconf to load a configuration from a string.
void from_string(void)
{
	struct config_example *config = malloc(sizeof(struct config_example));

	// Defaults
	strcpy(config->hostname, "Default hostname");
	config->public   = 0;

	char *config_string =
		"config ("
		" (hostname 'differentHostname')"
		" (public: no)"
		")";

	printf("config->hostname: %s\n", config->hostname);

	int ret_val;
	sconf_load_string(config_example, config, 0, config_string, ret_val);

	printf("config->hostname: %s\n", config->hostname);

	printf("return value: %d\n", ret_val);
}

/*
// Example of using sconf to load a configuration from a file
void from_file(void)
{
	struct file_example *config = malloc(sizeof(struct config_example));

	// Defaults
	config->debug_mode = 0;

	sconf_load_string(file_example, config, 0, "(config (debug_mode :no))");
}*/

int
main(int argc, char **argv)
{
	from_string();
	//from_file();
}
