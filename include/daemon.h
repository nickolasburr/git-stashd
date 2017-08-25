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

void fork_proc(void);
void write_log_file(char *filename, char *filemode);

#endif /* GIT_STASHD_DAEMON_H */
