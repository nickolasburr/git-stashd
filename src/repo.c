/**
 * repo.c
 *
 * Copyright (C) 2017 Nickolas Burr <nickolasburr@gmail.com>
 */

#include "repo.h"

char *get_commit_hash_by_index (struct repository *r, int index) {
	const char *format = "/usr/bin/git -C %s show --no-patch --format='%%H' stash@{%s}";
	char *cmd, hash[40], line[40];
	FILE *fp;

	/**
	 * Allocate space for `cmd`, create
	 * formatted command with interpolated
	 * pathname, and open pipe stream.
	 */
	cmd = ALLOC(sizeof(char) * ((strlen(r->path) + 1) + (sizeof(char) + 1) + (strlen(format) + 1)));

	sprintf(cmd, format, r->path, (char) index);

	fp = popen(cmd, "r");

	if (is_null(fp)) {
		printf("Could not open pipe!\n");

		exit(EXIT_FAILURE);
	}

	while (!is_null(fgets(line, 40, fp))) {
		// Remove CR, LF, CRLF, etc.
		line[strcspn(line, "\r\n")] = 0;

		concat(hash, line);
	}

	/**
	 * Free allocated space for `cmd`,
	 * and close pipe stream.
	 */
	FREE(cmd);
	printf("get_commit_hash_by_index: hash -> %s\n", hash);

	return hash;
}

/**
 * Get all stash entries for a repository from repo struct.
 */
struct stash *get_stash (struct repository *r) {
	return r->stash;
}

/**
 * Set stash on repository struct.
 */
void set_stash (struct repository *r) {
	int i = 0;
	const char *format = "/usr/bin/git -C %s stash list --format='%%s'";
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
		// Remove CR, LF, CRLF, etc.
		line[strcspn(line, "\r\n")] = 0;

		copy(r->stash->entries[i]->hash, get_commit_hash_by_index(r, i));
		copy(r->stash->entries[i]->message, line);

		printf("set_stash: hash    -> %s\n", r->stash->entries[i]->hash);
		printf("set_stash: message -> %s\n", line);

		i++;
	}

	// Set length member on stash struct.
	r->stash->length = (i + 1);

	/**
	 * Free allocated space for `cmd`,
	 * and close pipe stream.
	 */
	FREE(cmd);
	pclose(fp);
}

/**
 * List stash entries.
 */
void list_entries (struct stash *s) {
	int i, length = s->length;

	for (i = 0; i < length; i += 1) {
		printf("list_entries: hash    -> %s\n", s->entries[i]->hash);
		printf("list_entries: message -> %s\n", s->entries[i]->message);
	}
}

/**
 * Check if the worktree is dirty.
 */
int is_worktree_dirty (struct repository *r) {
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
