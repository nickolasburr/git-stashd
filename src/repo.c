/**
 * repo.c
 *
 * Copyright (C) 2017 Nickolas Burr <nickolasburr@gmail.com>
 */

#include "repo.h"

/**
 * Get all stashes for a repository.
 */
struct stash *get_stashes (struct repo *r) {
	return r->stashes;
}

/**
 * Set a copy of all stashes for a repository in memory.
 */
void set_stashes (struct repo *r) {
	// @todo: Build this function out.
	char *cmd;
	FILE *fp;

	printf("set_stashes -> r->path -> %s\n", r->path);

	sprintf(cmd, "/usr/bin/git -C %s -- stash list", r->path);

	fp = popen(cmd, "r");

	if (!fp) {
		printf("Could not open pipe!\n");

		exit(EXIT_FAILURE);
	}

	while (fgets(r->stashes->entries, (sizeof(r->stashes->entries) - 1), fp) != NULL) {
		printf("%s\n", r->stashes->entries);
	}

	pclose(fp);
}


/**
 * Check if the worktree has changed since our last check.
 */
int has_worktree_changed (struct repo *r) {
	return 1;
}
