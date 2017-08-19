/**
 * fs.h
 *
 * Copyright (C) 2017 Nickolas Burr <nickolasburr@gmail.com>
 */

#ifndef GIT_STASHD_FS_H
#define GIT_STASHD_FS_H

#include <dirent.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "argv.h"
#include "common.h"

#ifdef __APPLE__
#include <limits.h>
#else
#include <linux/limits.h>
#endif

#define GIT_STASHD_CHECK_REPO_CMD "git rev-parse --git-dir >/dev/null"

DIR *get_dir(const char *path);
FILE *get_file(char *filename, char *filemode);

int is_dir(const char *path);
int is_file(const char *path);
int is_repo(const char *path);

#endif /* GIT_STASHD_FS_H */
