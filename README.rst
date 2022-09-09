sconf
=====

An attempt to implement a type-safe s-expression configuration parser library in C via compile time
macro magic.

sconf does not work, and as endeared as I am by the concept of this library, I suspect inherent
limitations of C preprocessor macros (primarily forbidding recursion) make it infeasible. But I
might take another look at it at a later date.

It was a fun peek into the C preprocessor nonetheless.

Example
-------

.. code-block:: c

	#include "sconf.h"

	struct config_example {
		// Sconf will stop reading the config field after 49 bytes.
		char field[50];
		int  boolean;
	}

	int main()
	{
		struct config_example *config;
		config->field  = "Default value";
		config->boolean = 1;

		sconf_load_string(config, NULL, "(config (field 'Another value') (boolean :no)");

		// Output: boolean: 0
		printf("boolean: %d", config->boolean);
	}

sconf is deliberately very simple, see :code:`examples/` for more examples.
