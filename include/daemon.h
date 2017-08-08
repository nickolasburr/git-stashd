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

#ifdef __APPLE__
#include <limits.h>
#else
#include <linux/limits.h>
#endif

struct repo_info {
	char *path;
	long pid;
};

int is_dir(const char *path);
int is_repo(const char *path);

void start_daemon(const char *repo_path, long *pid);
void stop_daemon(long *pid);

FILE *get_log_file(char *filename, char *filemode);
int write_log_file(char *filename, char *filemode);

#endif
