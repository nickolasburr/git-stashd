/**
 * argv.c
 *
 * Copyright (C) 2017 Nickolas Burr <nickolasburr@gmail.com>
 */

#include "argv.h"

/**
 * Print formatted usage message to stdout.
 */
void usage (void) {
	printf("Usage: git-stashd [OPTIONS]\n\nType 'man git-stashd' for details");

	exit(EXIT_SUCCESS);
}
