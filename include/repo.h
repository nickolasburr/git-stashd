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

struct entry {
	int index;
	char hash[40];
	char message[80];
};

struct stash {
	int length;
	struct entry *entries[50];
};

struct repository {
	char *path;
	struct stash *stash;
};

int is_repo(const char *path);
int is_worktree_dirty(struct repository *r);

struct entry *get_entry(struct stash *s, int index);
int *set_entry(struct stash *s);
void list_entries(struct stash *s);

struct stash *get_stash(struct repository *r);
void set_stash(struct repository *r);

char *get_commit_hash_by_index(struct repository *r, int index);

#endif /* GIT_STASHD_REPO_H */
