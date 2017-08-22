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
 * List stash entries.
 */
void list_stash (struct repo *r) {
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
	int index_status;
	char *diff_index_cmd, *update_index_cmd;
	const char *diff_index_fmt   = "/usr/bin/git -C %s diff-index --quiet HEAD --",
	           *update_index_fmt = "/usr/bin/git -C %s update-index -q --really-refresh";

	/**
	 * Allocate space for `diff_index_cmd`.
	 */
	diff_index_cmd = ALLOC(sizeof(char) * ((strlen(r->path) + 1) + (strlen(diff_index_fmt) + 1)));

	/**
	 * Allocate space for `update_index_cmd`.
	 */
	update_index_cmd = ALLOC(sizeof(char) * ((strlen(r->path) + 1) + (strlen(update_index_fmt) + 1)));

	sprintf(diff_index_cmd, diff_index_fmt, r->path);
	sprintf(update_index_cmd, update_index_fmt, r->path);

	/**
	 * Refresh the index before checking state.
	 */
	system(update_index_cmd);

	index_status = system(diff_index_cmd);

	/**
	 * Free allocated space for `diff_index_cmd`, `update_index_cmd`.
	 */
	FREE(diff_index_cmd);
	FREE(update_index_cmd);

	printf("is_worktree_dirty: `index_status` -> %d\n", index_status);

	if (index_status) {
		return 1;
	}

	return 0;
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
