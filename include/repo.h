/**
 * repo.h
 *
 * Copyright (C) 2017 Nickolas Burr <nickolasburr@gmail.com>
 */

#ifndef GIT_STASHD_REPO_H
#define GIT_STASHD_REPO_H

#include "common.h"
#include "daemon.h"
#include "mem.h"
#include "utils.h"

#define GIT_STASHD_CHECK_REPO_CMD "git rev-parse --git-dir >/dev/null"
#define GIT_STASHD_ENTRY_LINE_MAX 1024

struct stash {
	int length;
	char *entries;
};

struct repo {
	char *path;
	struct stash *stash;
};

int is_repo(const char *path, const char *cmd);
int has_worktree_changed(struct repo *r);
struct stash *get_stash(struct repo *r);
void set_stash(struct repo *r);

#endif /* GIT_STASHD_REPO_H */
