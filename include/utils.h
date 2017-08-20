/**
 * utils.h
 *
 * Copyright (C) 2017 Nickolas Burr <nickolasburr@gmail.com>
 */

#ifndef GIT_STASHD_UTILS_H
#define GIT_STASHD_UTILS_H

#include <string.h>
#include "common.h"

char *concat(char *buf, char *str);
char *copy(char *buf, char *str);

#endif /* GIT_STASHD_UTILS_H */
