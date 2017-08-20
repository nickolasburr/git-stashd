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
	char *cmd, line[GIT_STASHD_ENTRY_LINE_MAX];
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

	while (!is_null(fgets(line, GIT_STASHD_ENTRY_LINE_MAX, fp))) {
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
 * Check if the worktree is dirty.
 */
int is_worktree_dirty (struct repo *r) {
	return 1;
}

/**
 * Determine if a pathname points to a directory with a Git repository.
 */
int is_repo (const char *path) {
	int rev_parse;

	if (!is_dir(path)) {
		return 0;
	}

	rev_parse = system(GIT_STASHD_CHECK_REPO_CMD);

	/**
	 * If it was a clean exit, then we can
	 * infer we're inside a Git repository.
	 */
	if (!rev_parse) {
		return 1;
	}

	return 0;
}
