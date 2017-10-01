/**
 * git.c
 *
 * Copyright (C) 2017 Nickolas Burr <nickolasburr@gmail.com>
 */

#include "git.h"

/**
 * Check if the stash has an entry with an equivalent diff of the worktree.
 */
int has_coequal_entry (int *error, const char *path, struct git_stashd_stash *s) {
	int index;
	static const char *diff_stash_fmt = "/usr/bin/git -C %s diff --quiet --exit-code stash@{%d}";

	*error = 0;

	for (index = 0; index < s->length; index++) {
		int coequal_status;
		char *diff_stash_cmd;

		diff_stash_cmd = ALLOC(sizeof(char) * ((length(diff_stash_fmt) + NULL_BYTE) + (length(path) + NULL_BYTE) + (sizeof(int) + 1)));
		sprintf(diff_stash_cmd, diff_stash_fmt, path, index);

		/**
		 * Use `git-diff` to check if if the entry
		 * diff is equivalent to the worktree diff.
		 */
		if ((coequal_status = system(diff_stash_cmd)) == -1) {
			*error = 1;
		}

		FREE(diff_stash_cmd);

		/**
		 * If there is a matching entry, return its index.
		 */
		if (!coequal_status) {
			return index;
		}
	}

	return -1;
}

char *get_sha_by_index (int *error, const char *path, char *sha_buf, int index) {
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
	show_entry_cmd = ALLOC(sizeof(char) * ((length(format) + NULL_BYTE) + (length(path) + NULL_BYTE)));
	sprintf(show_entry_cmd, format, path, index);

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

/**
 * Get the name of current branch.
 */
char *get_current_branch (int *error, const char *path, char *ref_buf) {
	FILE *fp;
	int fp_err;
	static const char *format = "/usr/bin/git -C %s rev-parse --abbrev-ref HEAD";
	char *current_branch_cmd,
	     line[GIT_STASHD_REF_LENGTH_MAX];

	*error = 0;

	current_branch_cmd = ALLOC(sizeof(char) * ((length(format) + NULL_BYTE) + (length(path) + NULL_BYTE)));
	sprintf(current_branch_cmd, format, path);

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
 * Add new entry to stash.
 *
 * @todo: Use libgit2 for creating/applying stash entry.
 */
void add_stash_entry (int *error, const char *path, struct git_stashd_stash *s) {
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
	get_current_branch(&ref_error, path, ref_buf);
	get_timestamp(ts_buf);

	if (ref_error) {
		*error = 1;

		return;
	}

	entry_msg = ALLOC(sizeof(char) * ((length(mformat) + NULL_BYTE) + (length(ref_buf) + NULL_BYTE) + (length(ts_buf) + NULL_BYTE)));
	sprintf(entry_msg, mformat, ref_buf, ts_buf);

	/**
	 * Allocate space for `create_cmd`.
	 */
	create_cmd = ALLOC(sizeof(char) * ((length(cformat) + NULL_BYTE) + (length(path) + NULL_BYTE) + (length(entry_msg) + NULL_BYTE)));
	sprintf(create_cmd, cformat, path, entry_msg);

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

	store_cmd = ALLOC(sizeof(char) * ((length(sformat) + NULL_BYTE) + (length(path) + NULL_BYTE) + (length(entry_msg) + NULL_BYTE) + (length(sha_buf) + NULL_BYTE)));
	sprintf(store_cmd, sformat, path, entry_msg, sha_buf);

	/**
	 * @todo: Use libgit2 instead of system call.
	 *
	 * If it was a non-zero exit, set the error marker.
	 */
	if (is_error((entry_status = system(store_cmd)))) {
		*error = 1;
	}

	close_pipe(fp);
	FREE(create_cmd);
	FREE(store_cmd);
	FREE(entry_msg);
}

/**
 * Callback function for libgit2 `git_stash_foreach`. Used solely for calculating stash length.
 */
git_stash_cb *init_setup (size_t index, const char *message, const git_oid *stash_id, void *payload) {
	/**
	 * Increment payload int pointer.
	 */
	(*(int*) payload)++;

	return 0;
}

/**
 * Callback function for libgit2 `git_stash_foreach`. Initialize repository stash.
 */
git_stash_cb *init_stash (size_t index, const char *message, const git_oid *stash_id, void *payload) {
	int sha_err;
	char sha_buf[GIT_STASHD_SHA_LENGTH_MAX];

	/**
	 * Retrieve the SHA1 hash for the stash entry.
	 */
	get_sha_by_index(&sha_err, ((struct git_stashd_stash *) payload)->repository->path, sha_buf, (int) index);

	if (sha_err) {
		fprintf(stderr, "Error encountered when calling get_sha_by_index\n");

		exit(EXIT_FAILURE);
	}

	copy(((struct git_stashd_stash *) payload)->entries[index]->hash, sha_buf);
	copy(((struct git_stashd_stash *) payload)->entries[index]->message, message);

	return 0;
}

/**
 * Get absolute path to .git directory.
 *
 * @todo: Refactor variable usage.
 */
char *get_git_dir (int *error, const char *path) {
	FILE *fp;
	int fp_err;
	char abs_path[PATH_MAX],
	     git_dir[PATH_MAX],
	     git_dir_buf[PATH_MAX],
	     top_dir[PATH_MAX],
	     top_dir_buf[PATH_MAX],
	     *git_dir_cmd,
	     *top_dir_cmd;
	/**
	 * @todo: These should be macros or static externs.
	 */
	static const char *git_dir_fmt = "/usr/bin/git -C %s rev-parse --git-dir",
	                  *top_dir_fmt = "/usr/bin/git -C %s rev-parse --show-toplevel";

	*error = 0;

	git_dir_cmd = ALLOC(sizeof(char) * ((length(path) + NULL_BYTE) + (length(git_dir_fmt) + NULL_BYTE)));
	sprintf(git_dir_cmd, git_dir_fmt, path);

	fp = open_pipe(&fp_err, git_dir_cmd, "r");

	if (fp_err) {
		goto on_error;
	}

	while (!is_null(fgets(git_dir_buf, PATH_MAX, fp))) {
		git_dir_buf[strcspn(git_dir_buf, "\r\n")] = 0;
	}

	copy(git_dir, git_dir_buf);

	/**
	 * Flush the stream.
	 */
	fflush(fp);

	top_dir_cmd = ALLOC(sizeof(char) * ((length(path) + NULL_BYTE) + (length(top_dir_fmt) + NULL_BYTE)));
	sprintf(top_dir_cmd, top_dir_fmt, path);

	fp = open_pipe(&fp_err, top_dir_cmd, "r");

	if (fp_err) {
		goto on_error;
	}

	while (!is_null(fgets(top_dir_buf, PATH_MAX, fp))) {
		top_dir_buf[strcspn(top_dir_buf, "\r\n")] = 0;
	}

	copy(top_dir, top_dir_buf);

	/**
	 * Assemble absolute path to .git directory.
	 */
	copy(abs_path, top_dir);
	concat(abs_path, "/");
	concat(abs_path, base_name(git_dir));

	close_pipe(fp);
	FREE(git_dir_cmd);
	FREE(top_dir_cmd);

	return abs_path;

on_error:
	*error = 1;

	close_pipe(fp);
	FREE(git_dir_cmd);
	FREE(top_dir_cmd);

	return abs_path;
}

/**
 * Check if a stashd.lock file exists for the repository.
 */
int has_lock (int *error, const char *path) {
	FILE *fp;
	int fp_err;
	char git_dir[PATH_MAX],
	     line[PATH_MAX],
	     lock_file[PATH_MAX],
	     *git_dir_cmd;
	/**
	 * @todo: This should be a macro or static extern.
	 */
	static const char *git_dir_fmt = "/usr/bin/git -C %s rev-parse --git-dir";

	*error = 0;

	git_dir_cmd = ALLOC(sizeof(char) * ((length(path) + NULL_BYTE) + (length(git_dir_fmt) + NULL_BYTE)));
	sprintf(git_dir_cmd, git_dir_fmt, path);

	fp = open_pipe(&fp_err, git_dir_cmd, "r");

	if (fp_err) {
		goto on_error;
	}

	while (!is_null(fgets(line, PATH_MAX, fp))) {
		line[strcspn(line, "\r\n")] = 0;
	}

	/**
	 * Get basename of $GIT_DIR.
	 */
	copy(git_dir, base_name(line));

	/**
	 * Assemble absolute path to stashd.lock file.
	 */
	copy(lock_file, path);
	concat(lock_file, "/");
	concat(lock_file, git_dir);
	concat(lock_file, "/");
	concat(lock_file, GIT_STASHD_LOCK_FILE);

	close_pipe(fp);
	FREE(git_dir_cmd);

	return is_file(lock_file);

on_error:
	*error = 1;

	close_pipe(fp);
	FREE(git_dir_cmd);

	return -1;
}

/**
 * Check if the worktree is dirty.
 *
 * @todo: Rebuild with libgit2 `git_diff_index_to_workdir`.
 */
int is_worktree_dirty (int *error, const char *path) {
	int index_status;
	char *diff_index_cmd, *update_index_cmd;
	/**
	 * @todo: These should be macros or static externs.
	 */
	static const char *diff_index_fmt   = "/usr/bin/git -C %s diff-index --quiet HEAD --",
	                  *update_index_fmt = "/usr/bin/git -C %s update-index -q --really-refresh";

	*error = 0;

	/**
	 * Allocate space for `diff_index_cmd`, `update_index_cmd`,
	 * format both strings for use with `system` builtin.
	 */
	diff_index_cmd = ALLOC(sizeof(char) * ((length(path) + NULL_BYTE) + (length(diff_index_fmt) + NULL_BYTE)));
	sprintf(diff_index_cmd, diff_index_fmt, path);

	update_index_cmd = ALLOC(sizeof(char) * ((length(path) + NULL_BYTE) + (length(update_index_fmt) + NULL_BYTE)));
	sprintf(update_index_cmd, update_index_fmt, path);

	/**
	 * Refresh the index before checking state,
	 * set error marker if an error occurred.
	 */
	if (is_error(system(update_index_cmd))) {
		*error = 1;
	}

	/**
	 * Get state information via `diff-index`,
	 * set error marker if an error occurred.
	 */
	if (is_error((index_status = system(diff_index_cmd)))) {
		*error = 1;
	}

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
