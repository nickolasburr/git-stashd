/**
 * repo.h
 *
 * Copyright (C) 2017 Nickolas Burr <nickolasburr@gmail.com>
 */

#ifndef GIT_STASHD_REPO_H
#define GIT_STASHD_REPO_H

#include "common.h"
#include "daemon.h"
#include "fs.h"
#include "mem.h"

struct stash {
	int length;
	char *entries;
};

struct repo {
	char *path;
	struct stash *stash;
};

struct stash *get_stash(struct repo *r);
void set_stash(struct repo *r);

int has_worktree_changed(struct repo *r);

#endif /* GIT_STASHD_REPO_H */
