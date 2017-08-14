/**
 * repo.c
 *
 * Copyright (C) 2017 Nickolas Burr <nickolasburr@gmail.com>
 */

#include "repo.h"

/**
 * Get all stashes for a repository.
 */
struct stash *get_stashes (struct repo *repo) {
	return repo->stashes;
}

/**
 * Set a copy of all stashes for a repository in memory.
 */
void set_stashes (struct repo *repo) {
	// @todo: Build this function out.
}

/**
 * Check if the worktree has changed since our last check.
 */
int has_worktree_changed (struct repo *repo) {
	return 1;
}
