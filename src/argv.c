/**
 * argv.c
 *
 * Copyright (C) 2017 Nickolas Burr <nickolasburr@gmail.com>
 */

#include "argv.h"

/**
 * Print formatted usage message to stdout
 */
void pfusage () {
	unsigned int i, pfindent = 5;
	const char* const options[GIT_STASHD_OPT_COUNT] = {
		"--help",
		"--foreground",
		"--interval",
		"--repository-path",
	};
	size_t size = (sizeof(options) / sizeof(options[0]));

	printf("%s\n\nOptions:\n\n", GIT_STASHD_OPT_HELP_M);

	for (i = 0; i < size; i += 1) {
		printf("%*s %s\n", pfindent, "", options[i]);
	}
}
