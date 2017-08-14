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
	struct stash stashes[];
};

struct stash *get_stashes(struct repo *repo);
void set_stashes(struct repo *repo);

int has_worktree_changed(struct repo *repo);

#endif /* GIT_STASHD_REPO_H */
