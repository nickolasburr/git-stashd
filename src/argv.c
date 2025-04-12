/**
 * argv.c
 *
 * Copyright (C) 2017 Nickolas Burr <nickolasburr@gmail.com>
 */

#include "argv.h"

/**
 * Print formatted usage message to stdout.
 */
void usage(void) {
	fprintf(
		stdout,
		"%s\n",
		GIT_STASHD_USAGE_TEXT
	);
	exit(EXIT_SUCCESS);
}
