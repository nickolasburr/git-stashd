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
 * Set a copy of all stash entries for a Git repository.
 */
void set_stash (struct repo *r) {
	const char *format = "/usr/bin/git -C %s stash list";
	char *cmd, line[STASH_ENTRY_LINE_MAX];
	FILE *fp;

	/**
	 * Allocate space for `cmd`, create
	 * formatted command with interpolated
	 * pathname, and open pipe stream.
	 */
	cmd = ALLOC(sizeof(char) * ((strlen(r->path) + 1) + (strlen(format) + 1)));

	sprintf(cmd, format, r->path);

	fp = popen(cmd, "r");

	if (!fp) {
		printf("Could not open pipe!\n");

		exit(EXIT_FAILURE);
	}

	while (fgets(line, STASH_ENTRY_LINE_MAX, fp) != NULL) {
		// Strip any existing newlines, carriage returns, etc.
		line[strcspn(line, "\r\n")] = 0;

		/**
		 * Append a single newline to the end of `line`,
		 * then merge `line` with `entries` char array.
		 */
		concat(line, "\n");
		concat(r->stash->entries, line);
	}

	/**
	 * Free allocated space for `cmd`,
	 * and close pipe stream.
	 */
	FREE(cmd);
	pclose(fp);
}


/**
 * Check if the worktree has changed since the last check.
 */
int has_worktree_changed (struct repo *r) {
	return 1;
}
