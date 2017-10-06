/**
 * argv.h
 *
 * Copyright (C) 2017 Nickolas Burr <nickolasburr@gmail.com>
 */

#ifndef GIT_STASHD_ARGV_H
#define GIT_STASHD_ARGV_H

#include "common.h"
#include "utils.h"

/**
 * @note: Consider removing these macros and using a struct.
 */
#define GIT_STASHD_OPT_HELP_M "git stashd [OPTIONS]"
#define GIT_STASHD_OPT_HELP_L "--help"
#define GIT_STASHD_OPT_HELP_S "-h"
#define GIT_STASHD_OPT_FOREGROUND_L "--foreground"
#define GIT_STASHD_OPT_FOREGROUND_S "-F"
#define GIT_STASHD_OPT_INTERVAL_L "--interval"
#define GIT_STASHD_OPT_INTERVAL_S "-I"
#define GIT_STASHD_OPT_LOG_FILE_L "--log-file"
#define GIT_STASHD_OPT_LOG_FILE_S "-L"
#define GIT_STASHD_OPT_MAX_ENTRIES_L "--max-entries"
#define GIT_STASHD_OPT_MAX_ENTRIES_S "-M"
#define GIT_STASHD_OPT_PATH_L "--path"
#define GIT_STASHD_OPT_PATH_S "-P"
#define GIT_STASHD_OPT_VERSION_L "--version"
#define GIT_STASHD_OPT_VERSION_S "-V"
#define GIT_STASHD_NUM_OPT_COUNT 14

static char *options[GIT_STASHD_NUM_OPT_COUNT];

int has_bad_opts(char **, int);
void usage(void);

#endif /* GIT_STASHD_ARGV_H */
