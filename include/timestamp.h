/**
 * timestamp.h
 *
 * Copyright (C) 2017 Nickolas Burr <nickolasburr@gmail.com>
 */

#ifndef GIT_STASHD_TIMESTAMP_H
#define GIT_STASHD_TIMESTAMP_H

#include <time.h>
#include "common.h"
#include "mem.h"
#include "repo.h"
#include "utils.h"

char *get_timestamp(char *ts_buf);

#endif /* GIT_STASHD_TIMESTAMP_H */
