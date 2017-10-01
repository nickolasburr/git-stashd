/**
 * common.h
 *
 * Copyright (C) 2017 Nickolas Burr <nickolasburr@gmail.com>
 */

#ifndef GIT_STASHD_COMMON_H
#define GIT_STASHD_COMMON_H

#include <ctype.h>
#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "error.h"

#ifdef __APPLE__
#include <limits.h>
#else
#include <linux/limits.h>
#endif

#define _GNU_SOURCE
#define NULL_BYTE 1
#define NULL_DEVICE "/dev/null"
#define NOT_FOUND -1
#define ERR_CATCH -2

extern char lock_file[PATH_MAX];
extern char log_path[PATH_MAX];

#endif /* GIT_STASHD_COMMON_H */
