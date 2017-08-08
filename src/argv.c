/**
 * argv.c
 *
 * Copyright (C) 2017 Nickolas Burr <nickolasburr@gmail.com>
 */

#include "common.h"
#include "argv.h"

/**
 * Get index of option string in array
 */
int opt_get_index (const char* const option, const char *arr[], size_t size) {
	int i;

	for (i = 0; i < size; i += 1) {
		if (!strcmp(arr[i], option)) {
			return i;
		}
	}

	return -1;
}

/**
 * Check if option exists in array
 */
int opt_in_array (const char* const option, const char *arr[], size_t size) {
	int i;

	for (i = 0; i < size; i += 1) {
		if (!strcmp(arr[i], option)) {
			return 1;
		}
	}

	return 0;
}
