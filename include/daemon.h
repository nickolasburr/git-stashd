/**
 * daemon.h
 *
 * Copyright (C) 2017 Nickolas Burr <nickolasburr@gmail.com>
 */

#ifndef GIT_STASHD_DAEMON_H
#define GIT_STASHD_DAEMON_H

#include "common.h"
#include "signals.h"
#include "utils.h"

#define GIT_STASHD_LOCK_FILE "stashd.lock"
#define GIT_STASHD_LOCK_MODE "w+"
#define GIT_STASHD_LOG_FILE "git-stashd.log"
#define GIT_STASHD_LOG_MODE "a+"
#define GIT_STASHD_INTERVAL 600
#define GIT_STASHD_MAX_ENTRIES 100

#define GIT_STASHD_ADD_ENTRY_TO_STASH_ERROR "--> Error encountered when adding entry to stash."
#define GIT_STASHD_CHECK_INDEX_STATUS_ERROR "--> Encountered an error when checking the index status."
#define GIT_STASHD_SEARCH_EQUIV_ENTRY_ERROR "--> Error encountered when searching for equivalent entry."
#define GIT_STASHD_WORKTREE_CLEAN_NO_ACTION "--> Worktree is clean, no action taken."
#define GIT_STASHD_WORKTREE_DIRTY_NEW_ENTRY "--> Worktree is dirty, no equivalent entry. Adding new entry."

void daemonize(void);
void touch_file(int*, char*, const char*);
void write_to_log(int*, const char*, const char*, const char*);

#endif /* GIT_STASHD_DAEMON_H */
