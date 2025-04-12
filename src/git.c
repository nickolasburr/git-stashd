/**
 * git.c
 *
 * Copyright (C) 2017 Nickolas Burr <nickolasburr@gmail.com>
 */

#include "git.h"

/**
 * Check if the stash has an entry with an equivalent diff of the worktree.
 */
int has_match_entry(
	int *error,
	const char *path,
	struct git_stashd_stash *s
) {
	int index, match;
	char *diff_stash_cmd = NULL;

	/**
	 * @todo: These should be macros or static externs.
	 */
	static const char *diff_stash_fmt = "/usr/bin/git -C %s diff --quiet --exit-code stash@{%d}";

	*error = 0;

	for (
		index = 0;
		index < s->length;
		index++
	) {
		diff_stash_cmd = ALLOC(
			sizeof(char) * (
				(strlen(diff_stash_fmt) + NUL_BYTE) +
				(strlen(path) + NUL_BYTE) +
				(sizeof(int) + NUL_BYTE)
			)
		);
		sprintf(
			diff_stash_cmd,
			diff_stash_fmt,
			path,
			index
		);

		match = system(diff_stash_cmd);

		/**
		 * Use `git-diff` to check if if the entry
		 * diff is equivalent to the worktree diff.
		 */
		if (match == -1) {
			*error = 1;
		}

		FREE(diff_stash_cmd);

		/**
		 * If there is a matching entry, return its index.
		 */
		if (!match) {
			return index;
		}
	}

	return -1;
}

char *get_sha_by_index(
	int *error,
	const char *path,
	char *sha_buf,
	int index
) {
	FILE *file = NULL;
	int fp_err;
	char *git_cmd = NULL,
		 *result = NULL,
	     line[GIT_STASHD_SHA_LENGTH_MAX];

	/**
	 * @todo: These should be macros or static externs.
	 */
	static const char *format = "/usr/bin/git -C %s show --no-patch --format='%%H' stash@{%d}";

	*error = 0;

	/**
	 * ALLOC space for `git_cmd`, create
	 * formatted command with interpolated
	 * pathname, and open pipe stream.
	 */
	git_cmd = ALLOC(
		sizeof(char) * (
			(strlen(format) + NUL_BYTE) +
			(strlen(path) + NUL_BYTE)
		)
	);
	sprintf(
		git_cmd,
		format,
		path,
		index
	);

	file = open_pipe(
		&fp_err,
		git_cmd,
		"r"
	);

	if (fp_err) {
		pclose(file);
		*error = 1;

		return sha_buf;
	}

	do {
		result = fgets(
			line,
			GIT_STASHD_SHA_LENGTH_MAX,
			file
		);

		if (result == NULL) {
			break;
		}

		line[strcspn(line, CRLF)] = 0;
		strcpy(sha_buf, line);
	} while (1);

	pclose(file);
	FREE(git_cmd);

	return sha_buf;
}

/**
 * Get the name of current branch.
 */
char *get_current_branch(
	int *error,
	const char *path,
	char *ref_buf
) {
	FILE *file = NULL;
	int fp_err;
	char *git_cmd = NULL,
		 *result = NULL,
	     line[GIT_STASHD_REF_LENGTH_MAX];

	/**
	 * @todo: These should be macros or static externs.
	 */
	static const char *format = "/usr/bin/git -C %s rev-parse --abbrev-ref HEAD";

	*error = 0;

	git_cmd = ALLOC(
		sizeof(char) * (
			(strlen(format) + NUL_BYTE) +
			(strlen(path) + NUL_BYTE)
		)
	);
	sprintf(
		git_cmd,
		format,
		path
	);

	file = open_pipe(
		&fp_err,
		git_cmd,
		"r"
	);

	if (fp_err) {
		pclose(file);
		*error = 1;
		return ref_buf;
	}

	do {
		result = fgets(
			line,
			GIT_STASHD_SHA_LENGTH_MAX,
			file
		);

		if (result == NULL) {
			break;
		}

		line[strcspn(line, CRLF)] = 0;
		strcpy(ref_buf, line);
	} while (1);

	pclose(file);
	FREE(git_cmd);

	return ref_buf;
}

/**
 * Add new entry to stash.
 *
 * @todo: Use libgit2 for creating/applying stash entry.
 */
void add_stash_entry(
	int *error,
	const char *path,
	struct git_stashd_stash *s
) {
	FILE *file = NULL;
	int entry_status;
	int fp_err, ref_error;
	char *create_cmd = NULL,
	     *store_cmd = NULL,
	     *entry_msg = NULL,
		 *result = NULL,
	     line[GIT_STASHD_MSG_LENGTH_MAX],
	     ts_buf[GIT_STASHD_TMS_LENGTH_MAX],
	     ref_buf[GIT_STASHD_REF_LENGTH_MAX],
	     sha_buf[GIT_STASHD_SHA_LENGTH_MAX];

	/**
	 * @todo: These should be macros or static externs.
	 */
	static const char *cformat = "/usr/bin/git -C %s stash create '%s'",
	                  *sformat = "/usr/bin/git -C %s stash store --message '%s' %s",
	                  *mformat = "WIP on %s: git-stashd autostash @ %s";

	*error = 0;

	get_current_branch(
		&ref_error,
		path,
		ref_buf
	);
	get_timestamp(ts_buf);

	if (ref_error) {
		*error = 1;
		return;
	}

	entry_msg = ALLOC(
		sizeof(char) * (
			(strlen(mformat) + NUL_BYTE) +
			(strlen(ref_buf) + NUL_BYTE) +
			(strlen(ts_buf) + NUL_BYTE)
		)
	);
	sprintf(
		entry_msg,
		mformat,
		ref_buf,
		ts_buf
	);

	/**
	 * Allocate space for `create_cmd`.
	 */
	create_cmd = ALLOC(
		sizeof(char) * (
			(strlen(cformat) + NUL_BYTE) +
			(strlen(path) + NUL_BYTE) +
			(strlen(entry_msg) + NUL_BYTE)
		)
	);
	sprintf(
		create_cmd,
		cformat,
		path,
		entry_msg
	);

	file = open_pipe(
		&fp_err,
		create_cmd,
		"r"
	);

	if (fp_err) {
		pclose(file);
		*error = 1;
		return;
	}

	do {
		result = fgets(
			line,
			GIT_STASHD_SHA_LENGTH_MAX,
			file
		);

		if (result == NULL) {
			break;
		}

		line[strcspn(line, CRLF)] = 0;
		strcpy(sha_buf, line);
	} while (1);

	store_cmd = ALLOC(
		sizeof(char) * (
			(strlen(sformat) + NUL_BYTE) +
			(strlen(path) + NUL_BYTE) +
			(strlen(entry_msg) + NUL_BYTE) +
			(strlen(sha_buf) + NUL_BYTE)
		)
	);
	sprintf(
		store_cmd,
		sformat,
		path,
		entry_msg,
		sha_buf
	);

	entry_status = system(store_cmd);

	/**
	 * @todo: Use libgit2 instead of system call.
	 *
	 * If it was a non-zero exit, set the error marker.
	 */
	if (entry_status == -1) {
		*error = 1;
	}

	pclose(file);
	FREE(create_cmd);
	FREE(store_cmd);
	FREE(entry_msg);
}

/**
 * Callback function for libgit2 `git_stash_foreach`.
 * Used solely for calculating stash length.
 */
git_stash_cb *init_setup(
	size_t index,
	const char *msg,
	const git_oid *stash_id,
	void *payload
) {
	/**
	 * Increment payload int pointer.
	 */
	(*(int *) payload)++;
	return 0;
}

/**
 * Callback function for libgit2 `git_stash_foreach`.
 * Initialize repository stash.
 */
git_stash_cb *init_stash(
	size_t index,
	const char *msg,
	const git_oid *stash_id,
	void *payload
) {
	int sha_err;
	char sha_buf[GIT_STASHD_SHA_LENGTH_MAX];

	/**
	 * Retrieve the SHA1 hash for the stash entry.
	 */
	get_sha_by_index(
		&sha_err,
		((struct git_stashd_stash *) payload)->repository->path,
		sha_buf,
		(int) index
	);

	if (sha_err) {
		fprintf(
			stderr,
			"Error encountered when calling get_sha_by_index\n"
		);
		exit(EXIT_FAILURE);
	}

	strcpy(
		((struct git_stashd_stash *) payload)->entries[index]->hash,
		sha_buf
	);
	strcpy(
		((struct git_stashd_stash *) payload)->entries[index]->message,
		msg
	);

	return 0;
}

/**
 * Get absolute path to .git directory.
 *
 * @todo: Refactor variable usage.
 */
char *get_git_dir(
	int *error,
	const char *path
) {
	FILE *file = NULL;
	int fp_err;
	size_t substr;
	char abs_path[PATH_MAX],
	     git_dir[PATH_MAX],
	     git_dir_buf[PATH_MAX],
	     top_dir[PATH_MAX],
	     top_dir_buf[PATH_MAX],
	     *git_dir_cmd = NULL,
	     *top_dir_cmd = NULL,
		 *result = NULL;

	/**
	 * @todo: These should be macros or static externs.
	 */
	static const char *git_dir_fmt = "/usr/bin/git -C %s rev-parse --git-dir",
	                  *top_dir_fmt = "/usr/bin/git -C %s rev-parse --show-toplevel";

	*error = 0;

	git_dir_cmd = ALLOC(
		sizeof(char) * (
			(strlen(path) + NUL_BYTE) +
			(strlen(git_dir_fmt) + NUL_BYTE)
		)
	);
	sprintf(
		git_dir_cmd,
		git_dir_fmt,
		path
	);

	file = open_pipe(
		&fp_err,
		git_dir_cmd,
		"r"
	);

	if (fp_err) {
		goto on_error;
	}

	do {
		result = fgets(
			git_dir_buf,
			PATH_MAX,
			file
		);

		if (result == NULL) {
			break;
		}

		substr = strcspn(git_dir_buf, CRLF);
		git_dir_buf[substr] = 0;
	} while (1);

	strcpy(
		git_dir,
		git_dir_buf
	);

	/**
	 * Flush the stream.
	 */
	fflush(file);

	top_dir_cmd = ALLOC(
		sizeof(char) * (
			(strlen(path) + NUL_BYTE) +
			(strlen(top_dir_fmt) + NUL_BYTE)
		)
	);
	sprintf(
		top_dir_cmd,
		top_dir_fmt,
		path
	);

	file = open_pipe(
		&fp_err,
		top_dir_cmd,
		"r"
	);

	if (fp_err) {
		goto on_error;
	}

	do {
		result = fgets(
			top_dir_buf,
			PATH_MAX,
			file
		);

		if (result == NULL) {
			break;
		}

		top_dir_buf[strcspn(top_dir_buf, CRLF)] = 0;
	} while (1);

	strcpy(
		top_dir,
		top_dir_buf
	);

	/**
	 * Assemble absolute path to .git directory.
	 */
	strcpy(abs_path, top_dir);
	strcat(abs_path, "/");
	strcat(
		abs_path,
		base_name(git_dir)
	);

	pclose(file);
	FREE(git_dir_cmd);
	FREE(top_dir_cmd);

	return abs_path;

on_error:
	*error = 1;

	pclose(file);
	FREE(git_dir_cmd);
	FREE(top_dir_cmd);

	return abs_path;
}

/**
 * Check if a stashd.lock file exists for the repository.
 */
int has_lock(
	int *error,
	const char *path
) {
	FILE *file = NULL;
	int fp_err;
	size_t substr;
	char git_dir[PATH_MAX],
	     line[PATH_MAX],
	     lock_file[PATH_MAX],
	     *git_dir_cmd = NULL,
		 *result = NULL;

	/**
	 * @todo: These should be macros or static externs.
	 */
	static const char *git_dir_fmt = "/usr/bin/git -C %s rev-parse --git-dir";

	*error = 0;

	git_dir_cmd = ALLOC(
		sizeof(char) * (
			(strlen(path) + NUL_BYTE) +
			(strlen(git_dir_fmt) + NUL_BYTE)
		)
	);
	sprintf(
		git_dir_cmd,
		git_dir_fmt,
		path
	);

	file = open_pipe(
		&fp_err,
		git_dir_cmd,
		"r"
	);

	if (fp_err) {
		goto on_error;
	}

	do {
		result = fgets(
			line,
			PATH_MAX,
			file
		);

		if (result == NULL) {
			break;
		}

		substr = strcspn(line, CRLF);
		line[substr] = 0;
	} while (1);

	/**
	 * Get basename of $GIT_DIR.
	 */
	strcpy(
		git_dir,
		base_name(line)
	);

	/**
	 * Assemble absolute path to stashd.lock file.
	 */
	strcpy(lock_file, path);
	strcat(lock_file, "/");
	strcat(lock_file, git_dir);
	strcat(lock_file, "/");
	strcat(
		lock_file,
		GIT_STASHD_LOCK_FILE
	);

	pclose(file);
	FREE(git_dir_cmd);

	return is_file(lock_file);

on_error:
	*error = 1;

	pclose(file);
	FREE(git_dir_cmd);

	return -1;
}

/**
 * Check if the worktree is dirty.
 *
 * @todo: Rebuild with libgit2 `git_diff_index_to_workdir`.
 */
int is_worktree_dirty(
	int *error,
	const char *path
) {
	int index_status, update_status;
	char *diff_index_cmd = NULL,
	     *update_index_cmd = NULL;

	/**
	 * @todo: These should be macros or static externs.
	 */
	static const char *diff_index_fmt   = "/usr/bin/git -C %s diff-index --quiet HEAD --",
	                  *update_index_fmt = "/usr/bin/git -C %s update-index -q --really-refresh";

	*error = 0;

	/**
	 * ALLOC for `diff_index_cmd`, `update_index_cmd`,
	 * format strings for use with `system` builtin.
	 */
	diff_index_cmd = ALLOC(
		sizeof(char) * (
			(strlen(path) + NUL_BYTE) +
			(strlen(diff_index_fmt) + NUL_BYTE)
		)
	);
	sprintf(
		diff_index_cmd,
		diff_index_fmt,
		path
	);

	update_index_cmd = ALLOC(
		sizeof(char) * (
			(strlen(path) + NUL_BYTE) +
			(strlen(update_index_fmt) + NUL_BYTE)
		)
	);
	sprintf(
		update_index_cmd,
		update_index_fmt,
		path
	);

	/**
	 * Refresh the index before checking state
	 */
	update_status = system(update_index_cmd);

	if (update_status == -1) {
		*error = 1;
	}

	/**
	 * Get state information via `diff-index`
	 */
	index_status = system(diff_index_cmd);

	if (index_status == -1) {
		*error = 1;
	}

	FREE(diff_index_cmd);
	FREE(update_index_cmd);

	return index_status ? 1 : 0;
}
