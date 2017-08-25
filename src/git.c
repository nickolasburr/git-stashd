/**
 * repo.c
 *
 * Copyright (C) 2017 Nickolas Burr <nickolasburr@gmail.com>
 */

#include "repo.h"

char *get_hash_by_entry_index (struct repository *r, char *sha_buf, int index) {
	int error;
	static const char *format = "/usr/bin/git -C %s show --no-patch --format='%%H' stash@{%d}";
	char *cmd, line[GIT_STASHD_SHA_LENGTH_MAX];
	FILE *fp;

	/**
	 * Allocate space for `cmd`, create
	 * formatted command with interpolated
	 * pathname, and open pipe stream.
	 */
	cmd = ALLOC(sizeof(char) * ((strlen(r->path) + NULL_BYTE) + (strlen(format) + NULL_BYTE)));

	sprintf(cmd, format, r->path, index);

	fp = get_pipe(cmd, "r", &error);

	/**
	 * Exit failure if there was an error opening the pipe.
	 */
	if (error) {
		printf("Could not open pipe!\n");

		exit(EXIT_FAILURE);
	}

	while (!is_null(fgets(line, GIT_STASHD_SHA_LENGTH_MAX, fp))) {
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
	int error;
	static const char *format = "/usr/bin/git -C %s show --no-patch --format='%%s' stash@{%d}";
	char *cmd, line[GIT_STASHD_MSG_LENGTH_MAX];
	FILE *fp;

	/**
	 * Allocate space for `cmd`, create
	 * formatted command with interpolated
	 * pathname, and open pipe stream.
	 */
	cmd = ALLOC(sizeof(char) * ((strlen(r->path) + NULL_BYTE) + (strlen(format) + NULL_BYTE)));

	sprintf(cmd, format, r->path, index);

	fp = get_pipe(cmd, "r", &error);

	/**
	 * Exit failure if there was an error opening the pipe.
	 */
	if (error) {
		printf("Could not open pipe!\n");

		exit(EXIT_FAILURE);
	}

	while (!is_null(fgets(line, GIT_STASHD_MSG_LENGTH_MAX, fp))) {
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
 * Get name of current branch.
 */
char *get_current_branch (struct repository *r, char *ref_buf) {
	int error;
	static const char *format = "/usr/bin/git -C %s rev-parse --abbrev-ref HEAD";
	char *current_branch_cmd, line[GIT_STASHD_REF_LENGTH_MAX];
	FILE *fp;

	current_branch_cmd = ALLOC(sizeof(char) * ((strlen(format) + NULL_BYTE) + (strlen(r->path) + NULL_BYTE)));
	sprintf(current_branch_cmd, format, r->path);

	fp = get_pipe(current_branch_cmd, "r", &error);

	if (error) {
		printf("Could not open pipe!\n");

		exit(EXIT_FAILURE);
	}

	while (!is_null(fgets(line, GIT_STASHD_REF_LENGTH_MAX, fp))) {
		line[strcspn(line, "\r\n")] = 0;

		copy(ref_buf, line);
	}

	FREE(current_branch_cmd);

	return ref_buf;
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
	int entry_status, error;
	FILE *fp;
	char *create_cmd,
	     *store_cmd,
	     *entry_msg,
	     line[GIT_STASHD_MSG_LENGTH_MAX],
	     ts_buf[GIT_STASHD_TMS_LENGTH_MAX],
	     ref_buf[GIT_STASHD_REF_LENGTH_MAX],
	     sha_buf[GIT_STASHD_SHA_LENGTH_MAX];
	static const char *cformat = "/usr/bin/git -C %s stash create '%s'",
	                  *sformat = "/usr/bin/git -C %s stash store --message '%s' %s",
	                  *mformat = "WIP on %s: git-stashd autostash - %s";

	/**
	 * Get current branch name
	 */
	get_current_branch(r, ref_buf);

	/**
	 * Get timestamp
	 */
	get_timestamp(ts_buf);

	entry_msg = ALLOC(sizeof(char) * ((strlen(mformat) + NULL_BYTE) + (strlen(ref_buf) + NULL_BYTE) + (strlen(ts_buf) + NULL_BYTE)));
	sprintf(entry_msg, mformat, ref_buf, ts_buf);

	/**
	 * Allocate space for `create_cmd`.
	 */
	create_cmd = ALLOC(sizeof(char) * ((strlen(cformat) + NULL_BYTE) + (strlen(r->path) + NULL_BYTE) + (strlen(entry_msg) + NULL_BYTE)));
	sprintf(create_cmd, cformat, r->path, entry_msg);

	fp = get_pipe(create_cmd, "r", &error);

	/**
	 * Exit failure if there was an error opening the pipe.
	 */
	if (error) {
		printf("Could not open pipe!\n");

		exit(EXIT_FAILURE);
	}

	while (!is_null(fgets(line, GIT_STASHD_SHA_LENGTH_MAX, fp))) {
		line[strcspn(line, "\r\n")] = 0;

		copy(sha_buf, line);
	}

	pclose(fp);

	/**
	 * Allocate space for `store_cmd`.
	 */
	store_cmd = ALLOC(sizeof(char) * ((strlen(sformat) + NULL_BYTE) + (strlen(r->path) + NULL_BYTE) + (strlen(entry_msg) + NULL_BYTE) + (strlen(sha_buf) + NULL_BYTE)));
	sprintf(store_cmd, sformat, r->path, entry_msg, sha_buf);

	entry_status = system(store_cmd);

	printf("set_entry: entry_status -> %d\n", entry_status);

	FREE(create_cmd);
	FREE(store_cmd);
	FREE(entry_msg);

	/**
	 * If it was a clean exit, then we'll get a return value
	 * of 0, and we can infer the entry was saved correctly.
	 */
	if (!entry_status) {
		return 1;
	}

	return 0;
}

/**
 * Get stash struct on repository struct.
 */
struct stash *get_stash (struct repository *r) {
	return r->stash;
}

/**
 * Set stash struct (and entries) on repository struct.
 */
void set_stash (struct repository *r) {
	int i, error;
	static const char *format = "/usr/bin/git -C %s stash list --format='%%s'";
	char *cmd, line[GIT_STASHD_MSG_LENGTH_MAX];
	FILE *fp;

	/**
	 * Allocate space for `cmd`, create
	 * formatted command with interpolated
	 * pathname, and open pipe stream.
	 */
	cmd = ALLOC(sizeof(char) * ((strlen(r->path) + NULL_BYTE) + (strlen(format) + NULL_BYTE)));

	sprintf(cmd, format, r->path);

	fp = get_pipe(cmd, "r", &error);

	/**
	 * Exit failure if there was an error opening the pipe.
	 */
	if (error) {
		printf("Could not open pipe!\n");

		exit(EXIT_FAILURE);
	}

	i = 0;

	while (!is_null(fgets(line, GIT_STASHD_MSG_LENGTH_MAX, fp))) {
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

	if (is_worktree_dirty(r)) {
		set_entry(r);
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
	 * @todo: These should be macros or static externs.
	 */
	static const char *diff_index_format   = "/usr/bin/git -C %s diff-index --quiet HEAD --",
	                  *update_index_format = "/usr/bin/git -C %s update-index -q --really-refresh";

	/**
	 * Allocate space for `diff_index_cmd`, `update_index_cmd`.
	 */
	diff_index_cmd   = ALLOC(sizeof(char) * ((strlen(r->path) + NULL_BYTE) + (strlen(diff_index_format) + NULL_BYTE)));
	update_index_cmd = ALLOC(sizeof(char) * ((strlen(r->path) + NULL_BYTE) + (strlen(update_index_format) + NULL_BYTE)));

	sprintf(diff_index_cmd, diff_index_format, r->path);
	sprintf(update_index_cmd, update_index_format, r->path);

	/**
	 * Refresh the index before checking state.
	 */
	system(update_index_cmd);

	/**
	 * Get state information via `diff-index`.
	 */
	index_status = system(diff_index_cmd);

	/**
	 * Free allocated space for `diff_index_cmd`, `update_index_cmd`.
	 */
	FREE(diff_index_cmd);
	FREE(update_index_cmd);

	/**
	 * If there exists changes that could be staged.
	 */
	if (index_status) {
		return 1;
	}

	return 0;
}

/**
 * Determine if a pathname points to a directory with a Git repository.
 */
int is_repo (const char *path) {
	int is_valid;
	/**
	 * @todo: This should be a macro or static extern.
	 */
	static const char *format = "/usr/bin/git -C %s rev-parse --git-dir >/dev/null 2>&1";
	char *check_repo_cmd;

	/**
	 * `is_valid` should be well-defined.
	 */
	is_valid = 0;

	/**
	 * Allocate space for `check_repo_cmd`.
	 */
	check_repo_cmd = ALLOC(sizeof(char) * ((strlen(path) + NULL_BYTE) + (strlen(format) + NULL_BYTE)));

	/**
	 * Verify `path` is a valid, readable directory.
	 */
	if (is_dir(path)) {
		/**
		 * If it was a clean exit, then we'll get a return value
		 * of 0, and we can infer we're inside a Git repository.
		 */
		is_valid = !system(check_repo_cmd);
	}

	/**
	 * Free space allocated for `check_repo_cmd`.
	 */
	FREE(check_repo_cmd);

	return is_valid;
}
