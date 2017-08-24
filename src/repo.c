/**
 * repo.c
 *
 * Copyright (C) 2017 Nickolas Burr <nickolasburr@gmail.com>
 */

#include "repo.h"

char *get_hash_by_entry_index (struct repository *r, char *sha_buf, int index) {
	static const char *format = "/usr/bin/git -C %s show --no-patch --format='%%H' stash@{%d}";
	char *cmd, line[GIT_STASHD_SHA_LENGTH_MAX];
	FILE *fp;

	/**
	 * Allocate space for `cmd`, create
	 * formatted command with interpolated
	 * pathname, and open pipe stream.
	 */
	cmd = ALLOC(sizeof(char) * ((strlen(r->path) + 1) + (strlen(format) + 1)));

	sprintf(cmd, format, r->path, index);

	fp = popen(cmd, "r");

	if (is_null(fp)) {
		printf("Could not open pipe!\n");

		exit(EXIT_FAILURE);
	}

	while (!is_null(fgets(line, GIT_STASHD_SHA_LENGTH_MAX, fp))) {
		// Remove CR, LF, CRLF, etc.
		line[strcspn(line, "\r\n")] = 0;

		copy(sha_buf, line);
	}

	/**
	 * Free allocated space for `cmd`,
	 * and close pipe stream.
	 */
	FREE(cmd);

	return sha_buf;
}

char *get_message_by_entry_index (struct repository *r, char *msg_buf, int index) {
	static const char *format = "/usr/bin/git -C %s show --no-patch --format='%%s' stash@{%d}";
	char *cmd, line[GIT_STASHD_MSG_LENGTH_MAX];
	FILE *fp;

	/**
	 * Allocate space for `cmd`, create
	 * formatted command with interpolated
	 * pathname, and open pipe stream.
	 */
	cmd = ALLOC(sizeof(char) * ((strlen(r->path) + 1) + (strlen(format) + 1)));

	sprintf(cmd, format, r->path, index);

	fp = popen(cmd, "r");

	if (is_null(fp)) {
		printf("Could not open pipe!\n");

		exit(EXIT_FAILURE);
	}

	while (!is_null(fgets(line, GIT_STASHD_MSG_LENGTH_MAX, fp))) {
		// Remove CR, LF, CRLF, etc.
		line[strcspn(line, "\r\n")] = 0;

		copy(msg_buf, line);
	}

	/**
	 * Free allocated space for `cmd`,
	 * and close pipe stream.
	 */
	FREE(cmd);

	return msg_buf;
}

/**
 * Get a stash entry by its index.
 */
struct entry *get_entry (struct stash *s, int index) {
	return s->entries[index];
}

/**
 * Create a stash entry, set it on stash->entries member.
 */
int set_entry (struct repository *r) {
	int entry_status;
	static const char *format = "/usr/bin/git -C %s stash create '%s'";
	char *msg = "WIP on branchname: Testing new autostash feature";
	char *cmd, line[GIT_STASHD_MSG_LENGTH_MAX];
	FILE *fp;

	/**
	 * Allocate space for `cmd`, create
	 * formatted command with interpolated
	 * pathname, and open pipe stream.
	 */
	cmd = ALLOC(sizeof(char) * ((strlen(r->path) + 1) + (strlen(format) + 1) + (strlen(msg) + 1)));

	sprintf(cmd, format, r->path, msg);

	entry_status = system(cmd);

	FREE(cmd);

	if (!entry_status) {
		return 1;
	}

	return 0;
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
	static const char *format = "/usr/bin/git -C %s stash list --format='%%s'";
	char *cmd, line[GIT_STASHD_MSG_LENGTH_MAX];
	FILE *fp;

	/**
	 * Allocate space for `cmd`, create
	 * formatted command with interpolated
	 * pathname, and open pipe stream.
	 */
	cmd = ALLOC(sizeof(char) * ((strlen(r->path) + 1) + (strlen(format) + 1)));

	sprintf(cmd, format, r->path);

	fp = popen(cmd, "r");

	if (is_null(fp)) {
		printf("Could not open pipe!\n");

		exit(EXIT_FAILURE);
	}

	while (!is_null(fgets(line, GIT_STASHD_MSG_LENGTH_MAX, fp))) {
		// Remove CR, LF, CRLF, etc.
		line[strcspn(line, "\r\n")] = 0;

		char *sha_buf = ALLOC(sizeof(char) * GIT_STASHD_SHA_LENGTH_MAX);
		char *msg_buf = ALLOC(sizeof(char) * GIT_STASHD_MSG_LENGTH_MAX);

		get_hash_by_entry_index(r, sha_buf, i);
		get_message_by_entry_index(r, msg_buf, i);

		copy(r->stash->entries[i]->hash, sha_buf);
		copy(r->stash->entries[i]->message, msg_buf);

		FREE(sha_buf);
		FREE(msg_buf);

		i++;
	}

	// Set length member on stash struct.
	r->stash->length = i;

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
	/**
	 * @todo: These should be macros or static globals.
	 */
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

	if (index_status) {
		return 1;
	}

	return 0;
}

/**
 * Determine if a pathname points to a directory with a Git repository.
 */
int is_repo (const char *path) {
	/**
	 * @todo: Properly build this out.
	 */
	static const char *format = "/usr/bin/git -C %s rev-parse --git-dir >/dev/null";

	/**
	 * Verify `path` is a valid, readable directory.
	 */
	if (is_dir(path)) {

		/**
		 * If it was a clean exit, then we can
		 * infer we're inside a Git repository.
		 */
		if (!system(GIT_STASHD_CHECK_REPO_CMD)) {
			return 1;
		}
	}

	return 0;
}
