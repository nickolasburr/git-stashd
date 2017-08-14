/**
 * main.h
 *
 * Copyright (C) 2017 Nickolas Burr <nickolasburr@gmail.com>
 */

#ifndef GIT_STASHD_MAIN_H
#define GIT_STASHD_MAIN_H

#include "argv.h"
#include "common.h"
#include "daemon.h"
#include "fs.h"
#include "repo.h"

FILE *fp;
int opt_index, arg_index, daemonize;
long pid;
char *pathname, *logfile;
char cwd[PATH_MAX];

#endif /* GIT_STASHD_MAIN_H */
