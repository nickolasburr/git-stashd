/**
 * git.h
 *
 * Copyright (C) 2017 Nickolas Burr <nickolasburr@gmail.com>
 */

#ifndef GIT_STASHD_GIT_H
#define GIT_STASHD_GIT_H

#include "common.h"
#include "daemon.h"
#include "error.h"
#include "mem.h"
#include "timestamp.h"
#include "utils.h"
#include "git2/checkout.h"
#include "git2/global.h"
#include "git2/oid.h"
#include "git2/repository.h"
#include "git2/stash.h"

#define GIT_STASHD_ENTRY_LINE_MAX 1024
#define GIT_STASHD_ENT_LENGTH_MAX 50
#define GIT_STASHD_MSG_LENGTH_MAX 90
#define GIT_STASHD_REF_LENGTH_MAX 255
#define GIT_STASHD_SHA_LENGTH_MAX 42
#define GIT_STASHD_TMS_LENGTH_MAX 18

struct git_stashd_entry;
struct git_stashd_stash;
struct git_stashd_repository;

struct git_stashd_entry {
	size_t index;
	char hash[GIT_STASHD_SHA_LENGTH_MAX];
	char message[GIT_STASHD_MSG_LENGTH_MAX];
	struct git_stashd_stash *stash;
};

struct git_stashd_stash {
	size_t length;
	struct git_stashd_entry *entries[GIT_STASHD_ENT_LENGTH_MAX];
	struct git_stashd_repository *repository;
};

struct git_stashd_repository {
	char path[PATH_MAX];
	struct git_stashd_stash *stash;
};

void add_stash_entry(int *error, const char *path, struct git_stashd_stash *s);
char *get_git_dir(int *, const char *);
int has_coequal_entry(int *error, const char *path, struct git_stashd_stash *s);
int has_lock(int *error, const char *path);
int is_worktree_dirty(int *error, const char *path);

git_stash_cb *init_setup(size_t index, const char *message, const git_oid *stash_id, void *payload);
git_stash_cb *init_stash(size_t index, const char *message, const git_oid *stash_id, void *payload);

char *get_current_branch(int *error, const char *path, char *ref_buf);
char *get_sha_by_index(int *error, const char *path, char *sha_buf, int index);

#endif /* GIT_STASHD_GIT_H */
