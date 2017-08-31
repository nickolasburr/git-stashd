/**
 * git.h
 *
 * Copyright (C) 2017 Nickolas Burr <nickolasburr@gmail.com>
 */

#ifndef GIT_STASHD_GIT_H
#define GIT_STASHD_GIT_H

#include "common.h"
#include "daemon.h"
#include "mem.h"
#include "timestamp.h"
#include "utils.h"

#define GIT_STASHD_ENTRY_LINE_MAX 1024
#define GIT_STASHD_ENT_LENGTH_MAX 50
#define GIT_STASHD_MSG_LENGTH_MAX 90
#define GIT_STASHD_REF_LENGTH_MAX 255
#define GIT_STASHD_SHA_LENGTH_MAX 42
#define GIT_STASHD_TMS_LENGTH_MAX 18

struct repository;
struct stash;
struct entry {
	int index;
	char hash[GIT_STASHD_SHA_LENGTH_MAX];
	char message[GIT_STASHD_MSG_LENGTH_MAX];
	struct stash *stash;
};

struct stash {
	int length;
	struct entry *entries[GIT_STASHD_ENT_LENGTH_MAX];
	struct repository *repo;
};

struct repository {
	char path[PATH_MAX];
	struct stash *stash;
};

int is_repo(char *path);
int is_worktree_dirty(int *error, struct repository *r);

void add_entry(int *error, struct stash *s);
int has_coequal_entry(int *error, struct stash *s);
void init_stash(int *error, struct repository *r);

char *get_current_branch(int *error, struct repository *r, char *ref_buf);
char *get_msg_by_index(int *error, struct stash *s, char *msg_buf, int index);
char *get_sha_by_index(int *error, struct stash *s, char *sha_buf, int index);

#endif /* GIT_STASHD_GIT_H */
