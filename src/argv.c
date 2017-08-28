/**
 * argv.c
 *
 * Copyright (C) 2017 Nickolas Burr <nickolasburr@gmail.com>
 */

#include "argv.h"

/**
 * Get index of option string in array
 */
int opt_get_index (const char* const option, const char *arr[], size_t size) {
	unsigned int i;

	for (i = 0; i < size; i += 1) {
		if (!compare(arr[i], option)) {
			return i;
		}
	}

	return -1;
}

/**
 * Check if option exists in array
 */
int opt_in_array (const char* const option, const char *arr[], size_t size) {
	unsigned int i;

	for (i = 0; i < size; i += 1) {
		if (!compare(arr[i], option)) {
			return 1;
		}
	}

	return 0;
}

/**
 * Print formatted usage message to stdout
 */
void pfusage () {
	unsigned int i, pfindent = 5;
	const char* const options[GIT_STASHD_OPT_COUNT] = {
		"--help",
		"--daemon",
		"--interval",
		"--repository-path",
	};
	size_t size = (sizeof(options) / sizeof(options[0]));

	printf("%s\n\nOptions:\n\n", GIT_STASHD_OPT_HELP_M);

	for (i = 0; i < size; i += 1) {
		printf("%*s %s\n", pfindent, "", options[i]);
	}
}
