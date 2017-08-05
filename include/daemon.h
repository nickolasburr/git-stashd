/**
 * daemon.h
 *
 * Copyright (C) 2017 Nickolas Burr <nickolasburr@gmail.com>
 */

#ifndef GIT_STASHD_DAEMON_H
#define GIT_STASHD_DAEMON_H

#include <dirent.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>

int is_dir(const char *path);
int is_repo(const char *path);
void stashd(const char *path);

#endif
