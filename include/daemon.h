/**
 * daemon.h
 *
 * Copyright (C) 2017 Nickolas Burr <nickolasburr@gmail.com>
 */

#ifndef GIT_STASHD_DAEMON_H
#define GIT_STASHD_DAEMON_H

#include "common.h"
#include "repo.h"
#include "signal.h"
#include "utils.h"

void fork_proc(void);
FILE *get_log_file(char *filename, char *filemode);
void write_log_file(char *filename, char *filemode);

#endif /* GIT_STASHD_DAEMON_H */
