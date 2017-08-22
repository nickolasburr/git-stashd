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

int is_repo(const char *path);
int is_worktree_dirty(struct repo *r);

struct stash *get_entry();
struct stash *set_entry();

struct stash *get_stash(struct repo *r);
struct stash *set_stash(struct repo *r);
void list_stash(struct repo *r);

char *get_commit_sha_by_index(struct repo *r);
char *is_same_entry(struct repo *r);

#endif /* GIT_STASHD_REPO_H */
