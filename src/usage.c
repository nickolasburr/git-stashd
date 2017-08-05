/**
 * usage.c
 *
 * Copyright (C) 2017 Nickolas Burr <nickolasburr@gmail.com>
 */

#include "usage.h"

const char* const options[GIT_STASHD_NUM_OPTS] = {
	"--help",
	"--daemon",
	"--interval",
	"--repository-path",
};

/* Options indentation level in usage output */
int pfindent = 5;

size_t size = (sizeof(options) / sizeof(options[0]));

/**
 * Print formatted usage message
 */
void pfusage () {
	int i;

	printf("%s\n\nOptions:\n\n", GIT_STASHD_USAGE_MSG);

	for (i = 0; i < size; i += 1) {
		printf("%*s %s\n", pfindent, "", options[i]);
	}
}
