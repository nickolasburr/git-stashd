/**
 * signal.h
 *
 * Copyright (C) 2017 Nickolas Burr <nickolasburr@gmail.com>
 */

#ifndef GIT_STASHD_SIGNALS_H
#define GIT_STASHD_SIGNALS_H

#include <signal.h>
#include "common.h"
#include "utils.h"

void on_signal(int signal);
void on_awake(int signal);
void nap(int seconds);

#endif /* GIT_STASHD_SIGNALS_H */
