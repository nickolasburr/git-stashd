/**
 * daemon.h
 *
 * Copyright (C) 2017 Nickolas Burr <nickolasburr@gmail.com>
 */

#ifndef GIT_STASHD_DAEMON_H
#define GIT_STASHD_DAEMON_H

#include "common.h"
#include "git.h"
#include "signals.h"
#include "utils.h"

#define GIT_STASHD_DEFAULT_INTERVAL 600

void fork_proc(int *error);
void write_log_file(int *error, char *filename, char *filemode);

#endif /* GIT_STASHD_DAEMON_H */
