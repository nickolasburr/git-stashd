/**
 * repo.c
 *
 * Copyright (C) 2017 Nickolas Burr <nickolasburr@gmail.com>
 */
#include "repo.h"

/**
 * Get all stash entries for a repository.
 */
struct stash *get_stash (struct repo *r) {
	return r->stash;
}

/**
 * Set a copy of all stash entries for a repository.
 */
void set_stash (struct repo *r) {
	char *cmd;
	FILE *fp;

	// Allocate space for command string
	cmd = ALLOC(sizeof(char) * (strlen(r->path) + 100));

	sprintf(cmd, "/usr/bin/git -C %s stash list", r->path);
	printf("set_stashes -> cmd -> %s\n", cmd);

	fp = popen(cmd, "r");

	if (!fp) {
		printf("Could not open pipe!\n");

		exit(EXIT_FAILURE);
	}

	while (fgets(r->stash->entries, (sizeof(r->stash->entries) - 1), fp) != NULL) {
		printf("%s\n", r->stash->entries);
	}

	// Free space allocated for commnad string
	FREE(cmd);
	pclose(fp);
}


/**
 * Check if the worktree has changed since the last check.
 */
int has_worktree_changed (struct repo *r) {
	return 1;
}
