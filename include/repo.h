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

struct stash {
	int total;
	char *entries;
};

struct repo {
	char *path;
	struct stash *stashes;
};

struct stash *get_stashes(struct repo *r);
void set_stashes(struct repo *r);

int has_worktree_changed(struct repo *r);

#endif /* GIT_STASHD_REPO_H */
