/**
 * error.c
 *
 * Copyright (C) 2017 Nickolas Burr <nickolasburr@gmail.com>
 */

#include "error.h"

/**
 * Exit failure, with error message, file, and line number.
 */
void die (char *message, char *file, int line) {
	fprintf(stderr, "ERROR: %s, caught in %s on line %d", message, file, line);

	exit(EXIT_FAILURE);
}
