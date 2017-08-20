/**
 * utils.c
 *
 * Copyright (C) 2017 Nickolas Burr <nickolasburr@gmail.com>
 */

#include "utils.h"

/**
 * `strcat` wrapper
 */
char *concat (char *buf, char *str) {
	return strcat(buf, str);
}

/**
 * `strcpy` wrapper
 */
char *copy (char *buf, char *str) {
	return strcpy(buf, str);
}
