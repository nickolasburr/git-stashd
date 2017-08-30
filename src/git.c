/**
 * git.c
 *
 * Copyright (C) 2017 Nickolas Burr <nickolasburr@gmail.com>
 */

#include "git.h"

char *get_sha_by_index (int *error, struct stash *s, char *sha_buf, int index) {
	FILE *fp;
	int fp_err;
	static const char *format = "/usr/bin/git -C %s show --no-patch --format='%%H' stash@{%d}";
	char *show_entry_cmd,
	     line[GIT_STASHD_SHA_LENGTH_MAX];

	*error = 0;

	/**
	 * Allocate space for `show_entry_cmd`,
	 * create formatted command with interpolated
	 * pathname, and open pipe stream.
	 */
	show_entry_cmd = ALLOC(sizeof(char) * ((strlen(format) + NULL_BYTE) + (strlen(s->repo->path) + NULL_BYTE)));
	sprintf(show_entry_cmd, format, s->repo->path, index);

	fp = open_pipe(&fp_err, show_entry_cmd, "r");

	if (fp_err) {
		close_pipe(fp);
		*error = 1;

		return sha_buf;
	}

	while (!is_null(fgets(line, GIT_STASHD_SHA_LENGTH_MAX, fp))) {
		line[strcspn(line, "\r\n")] = 0;

		copy(sha_buf, line);
	}

	close_pipe(fp);
	FREE(show_entry_cmd);

	return sha_buf;
}

char *get_msg_by_index (int *error, struct stash *s, char *msg_buf, int index) {
	FILE *fp;
	int fp_err;
	static const char *format = "/usr/bin/git -C %s show --no-patch --format='%%s' stash@{%d}";
	char *show_entry_cmd,
	     line[GIT_STASHD_MSG_LENGTH_MAX];

	*error = 0;

	/**
	 * Allocate space for `show_entry_cmd`,
	 * create formatted command with interpolated
	 * pathname, and open pipe stream.
	 */
	show_entry_cmd = ALLOC(sizeof(char) * ((strlen(format) + NULL_BYTE) + (strlen(s->repo->path) + NULL_BYTE)));
	sprintf(show_entry_cmd, format, s->repo->path, index);

	fp = open_pipe(&fp_err, show_entry_cmd, "r");

	if (fp_err) {
		close_pipe(fp);
		*error = 1;

		return msg_buf;
	}

	while (!is_null(fgets(line, GIT_STASHD_MSG_LENGTH_MAX, fp))) {
		line[strcspn(line, "\r\n")] = 0;

		copy(msg_buf, line);
	}

	close_pipe(fp);
	FREE(show_entry_cmd);

	return msg_buf;
}

/**
 * Get name of current branch.
 */
char *get_current_branch (int *error, struct repository *r, char *ref_buf) {
	FILE *fp;
	int fp_err;
	static const char *format = "/usr/bin/git -C %s rev-parse --abbrev-ref HEAD";
	char *current_branch_cmd,
	     line[GIT_STASHD_REF_LENGTH_MAX];

	*error = 0;

	current_branch_cmd = ALLOC(sizeof(char) * ((strlen(format) + NULL_BYTE) + (strlen(r->path) + NULL_BYTE)));
	sprintf(current_branch_cmd, format, r->path);

	fp = open_pipe(&fp_err, current_branch_cmd, "r");

	if (fp_err) {
		close_pipe(fp);
		*error = 1;

		return ref_buf;
	}

	while (!is_null(fgets(line, GIT_STASHD_REF_LENGTH_MAX, fp))) {
		line[strcspn(line, "\r\n")] = 0;

		copy(ref_buf, line);
	}

	close_pipe(fp);
	FREE(current_branch_cmd);

	return ref_buf;
}

/**
 * Set existing entries on repo->stash->entries struct.
 */
void add_entry (int *error, struct stash *s) {
	FILE *fp;
	int entry_status,
	    fp_err,
	    ref_error;
	char *create_cmd,
	     *store_cmd,
	     *entry_msg,
	     line[GIT_STASHD_MSG_LENGTH_MAX],
	     ts_buf[GIT_STASHD_TMS_LENGTH_MAX],
	     ref_buf[GIT_STASHD_REF_LENGTH_MAX],
	     sha_buf[GIT_STASHD_SHA_LENGTH_MAX];
	static const char *cformat = "/usr/bin/git -C %s stash create '%s'",
	                  *sformat = "/usr/bin/git -C %s stash store --message '%s' %s",
	                  *mformat = "WIP on %s: git-stashd autostash @ %s";

	*error = 0;

	/**
	 * Get branch, timestamp
	 */
	get_current_branch(&ref_error, s->repo, ref_buf);
	get_timestamp(ts_buf);

	if (ref_error) {
		*error = 1;

		return;
	}

	entry_msg = ALLOC(sizeof(char) * ((strlen(mformat) + NULL_BYTE) + (strlen(ref_buf) + NULL_BYTE) + (strlen(ts_buf) + NULL_BYTE)));
	sprintf(entry_msg, mformat, ref_buf, ts_buf);

	/**
	 * Allocate space for `create_cmd`.
	 */
	create_cmd = ALLOC(sizeof(char) * ((strlen(cformat) + NULL_BYTE) + (strlen(s->repo->path) + NULL_BYTE) + (strlen(entry_msg) + NULL_BYTE)));
	sprintf(create_cmd, cformat, s->repo->path, entry_msg);

	fp = open_pipe(&fp_err, create_cmd, "r");

	if (fp_err) {
		close_pipe(fp);
		*error = 1;

		return;
	}

	while (!is_null(fgets(line, GIT_STASHD_SHA_LENGTH_MAX, fp))) {
		line[strcspn(line, "\r\n")] = 0;

		copy(sha_buf, line);
	}

	store_cmd = ALLOC(sizeof(char) * ((strlen(sformat) + NULL_BYTE) + (strlen(s->repo->path) + NULL_BYTE) + (strlen(entry_msg) + NULL_BYTE) + (strlen(sha_buf) + NULL_BYTE)));
	sprintf(store_cmd, sformat, s->repo->path, entry_msg, sha_buf);

	/**
	 * @todo: Use a better solution than system.
	 */
	entry_status = system(store_cmd);

	/**
	 * @debug
	 */
	uid_t euid = geteuid();
	int test_err;
	char *log_fmt = "geteuid -> %d, exit status is %d\n";
	char *log_msg = ALLOC(sizeof(char) * ((strlen(log_fmt) + NULL_BYTE) + (sizeof(int) + 2)));
	sprintf(log_msg, log_fmt, (int) euid, entry_status);

	write_log_file(&test_err, GIT_STASHD_LOG_FILE, GIT_STASHD_LOG_MODE, log_msg);

	/**
	 * If it was a non-zero exit, we can infer an error was encountered.
	 */
	if (entry_status) {
		*error = 1;
	}

	close_pipe(fp);
	FREE(create_cmd);
	FREE(store_cmd);
	FREE(entry_msg);
}

/**
 * Initialize stash struct (and entries) on repository struct.
 */
void init_stash (int *error, struct repository *r) {
	FILE *fp;
	int index, fp_err;
	static const char *format = "/usr/bin/git -C %s stash list --format='%%s'";
	char *stash_list_cmd,
	     line[GIT_STASHD_MSG_LENGTH_MAX];

	*error = 0;

	/**
	 * Allocate space for `stash_list_cmd`,
	 * create formatted command with interpolated
	 * pathname, and open pipe stream.
	 */
	stash_list_cmd = ALLOC(sizeof(char) * ((strlen(format) + NULL_BYTE) + (strlen(r->path) + NULL_BYTE)));
	sprintf(stash_list_cmd, format, r->path);

	fp = open_pipe(&fp_err, stash_list_cmd, "r");

	if (fp_err) {
		close_pipe(fp);
		*error = 1;

		return;
	}

	index = 0;

	while (!is_null(fgets(line, GIT_STASHD_MSG_LENGTH_MAX, fp))) {
		line[strcspn(line, "\r\n")] = 0;

		int msg_error, sha_error;
		char *sha_buf = ALLOC(sizeof(char) * GIT_STASHD_SHA_LENGTH_MAX),
		     *msg_buf = ALLOC(sizeof(char) * GIT_STASHD_MSG_LENGTH_MAX);

		get_msg_by_index(&msg_error, r->stash, msg_buf, index);
		get_sha_by_index(&sha_error, r->stash, sha_buf, index);

		if (!msg_error && !sha_error) {
			copy(r->stash->entries[index]->hash, sha_buf);
			copy(r->stash->entries[index]->message, msg_buf);
		}

		FREE(sha_buf);
		FREE(msg_buf);

		index++;
	}

	/**
	 * Length of stash entries.
	 */
	r->stash->length = index;

	close_pipe(fp);
	FREE(stash_list_cmd);
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
	 * Allocate space for `diff_index_cmd` and `update_index_cmd`,
	 * format both strings for use with `system` builtin.
	 */
	diff_index_cmd = ALLOC(sizeof(char) * ((strlen(r->path) + NULL_BYTE) + (strlen(diff_index_format) + NULL_BYTE)));
	sprintf(diff_index_cmd, diff_index_format, r->path);

	update_index_cmd = ALLOC(sizeof(char) * ((strlen(r->path) + NULL_BYTE) + (strlen(update_index_format) + NULL_BYTE)));
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
	 * Deallocate space for `diff_index_cmd`, `update_index_cmd`.
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
int is_repo (char *path) {
	int is_valid;
	char *check_repo_cmd;
	/**
	 * @todo: This should be a macro or static extern.
	 */
	static const char *format = "/usr/bin/git -C %s rev-parse --git-dir >/dev/null 2>&1";

	/**
	 * `is_valid` should be well-defined.
	 */
	is_valid = 0;

	/**
	 * Verify `path` is a valid, readable directory.
	 */
	if (is_dir(path)) {
		/**
		 * Allocate space for `check_repo_cmd`.
		 */
		check_repo_cmd = ALLOC(sizeof(char) * ((strlen(format) + NULL_BYTE) + (strlen(path) + NULL_BYTE)));

		/**
		 * If it was a clean exit, then we'll get a return value
		 * of 0, and we can infer we're inside a Git repository.
		 */
		is_valid = !system(check_repo_cmd);

		/**
		 * Free space allocated for `check_repo_cmd`.
		 */
		FREE(check_repo_cmd);
	}

	return is_valid;
}
