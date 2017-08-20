/**
 * utils.h
 *
 * Copyright (C) 2017 Nickolas Burr <nickolasburr@gmail.com>
 */

#ifndef GIT_STASHD_UTILS_H
#define GIT_STASHD_UTILS_H

#include <dirent.h>
#include <signal.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "common.h"

#ifdef __APPLE__
#include <limits.h>
#else
#include <linux/limits.h>
#endif

/**
 * String utilities
 */

char *concat(char *buf, char *str);
char *copy(char *buf, char *str);

/**
 * Filesystem utilities
 */

DIR *get_dir(const char *path);
FILE *get_file(const char *filename, const char *filemode);

int is_dir(const char *path);
int is_file(const char *path);
int is_link(const char *path);
int is_sock(const char *path);
int is_fifo(const char *path);
int is_block(const char *path);
int is_char(const char *path);

#endif /* GIT_STASHD_UTILS_H */
