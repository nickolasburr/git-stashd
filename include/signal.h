/**
 * signal.h
 *
 * Copyright (C) 2017 Nickolas Burr <nickolasburr@gmail.com>
 */

#ifndef GIT_STASHD_SIGNAL_H
#define GIT_STASHD_SIGNAL_H

#include <signal.h>
#include "common.h"

void on_signal(int signal);
void on_sigalrm(int signal);
void nap(int seconds);

#endif /* GIT_STASHD_SIGNAL_H */
