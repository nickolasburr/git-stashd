/**
 * argv.c
 *
 * Copyright (C) 2017 Nickolas Burr <nickolasburr@gmail.com>
 */

#include "common.h"
#include "argv.h"

/**
 * Check if element exists in array
 */
int in_array (const char* const option, const char *arr[], size_t size) {
	int i;

	for (i = 0; i < size; i += 1) {
		if (!strcmp(arr[i], option)) {
			return 1;
		}
	}

	return 0;
}
