/**
 * error.h
 *
 * Copyright (C) 2017 Nickolas Burr <nickolasburr@gmail.com>
 */

#ifndef GIT_STASHD_ERROR_H
#define GIT_STASHD_ERROR_H

#include "common.h"
#include "utils.h"

void die(char *error, char *file, int line);

#endif /* GIT_STASHD_ERROR_H */
