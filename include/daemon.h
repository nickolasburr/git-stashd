/**
 * daemon.h
 *
 * Copyright (C) 2017 Nickolas Burr <nickolasburr@gmail.com>
 */

#ifndef GIT_STASHD_DAEMON_H
#define GIT_STASHD_DAEMON_H

#include "common.h"
#include "signals.h"
#include "utils.h"

#define GIT_STASHD_LOG_DIR  "/var/log"
#define GIT_STASHD_LOG_FILE "/var/log/git-stashd.log"
#define GIT_STASHD_LOG_MODE "a+"

#define GIT_STASHD_INTERVAL 600

void fork_proc(void);
void touch_log_file(int *error, char *log_file, char *filemode);
void write_log_file(int *error, char *filename, char *filemode, char *message);

#endif /* GIT_STASHD_DAEMON_H */
