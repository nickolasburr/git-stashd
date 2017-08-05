/**
 * usage.h
 *
 * Copyright (C) 2017 Nickolas Burr <nickolasburr@gmail.com>
 */

#ifndef GIT_STASHD_USAGE_H
#define GIT_STASHD_USAGE_H

#include <stdio.h>

#define GIT_STASHD_USAGE_MSG "git stashd [OPTIONS]"
#define GIT_STASHD_USAGE_OPT "--help"

#define GIT_STASHD_NUM_OPTS  4

const char* const options[GIT_STASHD_NUM_OPTS];
size_t size;
int pfindent;

void pfusage();

#endif /* GIT_STASHD_USAGE_H */
