/**
 * argv.h
 *
 * Copyright (C) 2017 Nickolas Burr <nickolasburr@gmail.com>
 */

#ifndef GIT_STASHD_ARGV_H
#define GIT_STASHD_ARGV_H

#include "common.h"
#include "utils.h"

#define GIT_STASHD_OPT_HELP_M "git stashd [OPTIONS]"
#define GIT_STASHD_OPT_HELP_L "--help"
#define GIT_STASHD_OPT_HELP_S "-h"

#define GIT_STASHD_OPT_DAEMON_L "--daemon"
#define GIT_STASHD_OPT_DAEMON_S "-D"

#define GIT_STASHD_OPT_LOG_FILE_L "--log-file"
#define GIT_STASHD_OPT_LOG_FILE_S "-L"

#define GIT_STASHD_OPT_REPOPATH_L "--repository-path"
#define GIT_STASHD_OPT_REPOPATH_S "-P"

#define GIT_STASHD_OPT_INTERVAL_L "--interval"
#define GIT_STASHD_OPT_INTERVAL_S "-I"

#define GIT_STASHD_OPT_COUNT 5

const char* const options[GIT_STASHD_OPT_COUNT];
int pfindent;
size_t size;

int opt_get_index(const char* const option, const char *arr[], size_t size);
int opt_in_array(const char* const option, const char *arr[], size_t size);
void pfusage(void);

#endif /* GIT_STASHD_ARGV_H */
