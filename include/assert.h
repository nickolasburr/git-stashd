/**
 * assert.h
 *
 * Copyright (C) 2017 Nickolas Burr <nickolasburr@gmail.com>
 */

#ifndef GIT_STASHD_ASSERT_H
#define GIT_STASHD_ASSERT_H

#undef assert

#ifdef NDEBUG
#define assert(e) ((void)0)
#else
#include "except.h"
extern void assert(int e);
#define assert(e) ((void)((e)||(RAISE(Assert_Failed),0)))
#endif

#endif /* GIT_STASHD_ASSERT_H */
