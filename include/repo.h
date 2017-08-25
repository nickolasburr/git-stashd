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
#include "timestamp.h"
#include "utils.h"

#define GIT_STASHD_ENTRY_LINE_MAX 1024
#define GIT_STASHD_ENT_LENGTH_MAX 50
#define GIT_STASHD_MSG_LENGTH_MAX 90
#define GIT_STASHD_REF_LENGTH_MAX 255
#define GIT_STASHD_SHA_LENGTH_MAX 42
#define GIT_STASHD_TMS_LENGTH_MAX 18

struct entry {
	int index;
	char hash[GIT_STASHD_SHA_LENGTH_MAX];
	char message[GIT_STASHD_MSG_LENGTH_MAX];
};

struct stash {
	int length;
	struct entry *entries[GIT_STASHD_ENT_LENGTH_MAX];
};

struct repository {
	char path[PATH_MAX];
	struct stash *stash;
};

int is_repo(const char *path);
int is_worktree_dirty(struct repository *r);

struct entry *get_entry(struct stash *s, int index);
int set_entry(struct repository *r);
void list_entries(struct stash *s);

struct stash *get_stash(struct repository *r);
void set_stash(struct repository *r);

char *get_current_branch(struct repository *r, char *ref_buf);
char *get_hash_by_entry_index(struct repository *r, char *sha_buf, int index);
char *get_message_by_entry_index(struct repository *r, char *msg_buf, int index);

#endif /* GIT_STASHD_REPO_H */
