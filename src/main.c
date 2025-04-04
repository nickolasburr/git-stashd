/**
 * main.c
 *
 * Copyright (C) 2017 Nickolas Burr <nickolasburr@gmail.com>
 */

#include "main.h"

/**
 * @note: Externs lock_file, log_path declared in common.h.
 */
char lock_file[PATH_MAX];
char log_path[PATH_MAX];

int main (int argc, char **argv) {
	int index,
	    fp_err,
	    git_dir_err,
	    lock_err,
	    lock_fp_err,
	    log_fp_err,
	    init_err,
	    arg_index,
	    opt_index,
	    daemonized,
	    entry_status,
	    index_status,
	    interval,
	    last_index,
	    max_entries,
	    stash_length;
	char path_buf[PATH_MAX],
	     git_dir[PATH_MAX],
	     home_dir[PATH_MAX],
	     log_dir[PATH_MAX],
	     log_realpath[PATH_MAX],
	     s_interval[4],
	     s_max_entries[4],
	     *cwd,
	     *log_file,
	     *path;

	/**
	 * Set up struct initializers.
	 */
	git_repository *repo;
	struct git_stashd_stash *stash;
	struct git_stashd_repository *stash_repo;
	struct sigaction action;

	/**
	 * Catch bad options and display an error.
	 */
	if (has_bad_opts(argv, argc)) {
		fprintf(stdout, "Invalid options given\n");
		exit(EXIT_FAILURE);
	}

	/**
	 * If --help option was given, display usage details and exit.
	 */
	if (in_array(GIT_STASHD_OPT_HELP_L, argv, argc) ||
	    in_array(GIT_STASHD_OPT_HELP_S, argv, argc)) {
		usage();
	}

	/**
	 * If --version option was given, display version number and exit.
	 */
	if (in_array(GIT_STASHD_OPT_VERSION_L, argv, argc) ||
	    in_array(GIT_STASHD_OPT_VERSION_S, argv, argc)) {
		fprintf(stdout, "%s\n", GIT_STASHD_VERSION);
		exit(EXIT_SUCCESS);
	}

	/**
	 * Initialize libgit2
	 */
	git_libgit2_init();

	/**
	 * Get the index of the last element in `argv`.
	 */
	last_index = (argc - 1);

	/**
	 * Daemonize by default.
	 */
	daemonized = 1;

	/**
	 * Get $HOME directory for user.
	 */
	copy(home_dir, getenv("HOME"));

	/**
	 * Check if `--foreground` option was given. This will determine
	 * if a child process should be forked from the parent process.
	 */
	if (in_array(GIT_STASHD_OPT_FOREGROUND_L, argv, argc) ||
	    in_array(GIT_STASHD_OPT_FOREGROUND_S, argv, argc)) {
		daemonized = 0;
	}

	/**
	 * Check if `--interval` option was given.
	 */
	if (in_array(GIT_STASHD_OPT_INTERVAL_L, argv, argc) ||
	    in_array(GIT_STASHD_OPT_INTERVAL_S, argv, argc)) {

		opt_index = (index_of(GIT_STASHD_OPT_INTERVAL_L, argv, argc) != NOT_FOUND)
		          ? index_of(GIT_STASHD_OPT_INTERVAL_L, argv, argc)
		          : index_of(GIT_STASHD_OPT_INTERVAL_S, argv, argc);

		if ((arg_index = (opt_index + 1)) > last_index) {
			fprintf(stderr, "--interval: Missing argument\n");
			exit(EXIT_FAILURE);
		}

		copy(s_interval, argv[arg_index]);

		/**
		 * Verify `--interval` option argument is a valid number.
		 */
		if (!is_numeric(s_interval)) {
			fprintf(stderr, "--interval: Invalid argument %s. Argument must be an integer.\n", s_interval);
			exit(EXIT_FAILURE);
		}

		interval = atoi(s_interval);

		/**
		 * Enforce strict lower bound of 10 seconds.
		 */
		if (interval < 10) {
			fprintf(stderr, "--interval: To prevent overload, interval must be >= 10.\n");
			exit(EXIT_FAILURE);
		}
	} else {
		interval = GIT_STASHD_INTERVAL;
	}

	/**
	 * Check if `--log-file` option was given.
	 */
	if (in_array(GIT_STASHD_OPT_LOG_FILE_L, argv, argc) ||
	    in_array(GIT_STASHD_OPT_LOG_FILE_S, argv, argc)) {

		opt_index = (index_of(GIT_STASHD_OPT_LOG_FILE_L, argv, argc) != NOT_FOUND)
		          ? index_of(GIT_STASHD_OPT_LOG_FILE_L, argv, argc)
		          : index_of(GIT_STASHD_OPT_LOG_FILE_S, argv, argc);

		if ((arg_index = (opt_index + 1)) > last_index) {
			fprintf(stderr, "--log-file: Missing argument\n");
			exit(EXIT_FAILURE);
		}

		/**
		 * Set realpath value on log_path.
		 */
		realpath(argv[arg_index], log_path);
		copy(log_dir, dir_name(base_name(log_path)));

		/**
		 * Check if `log_path` is an existing file, or if it needs to be created.
		 */
		if (!is_file(log_path)) {

			/**
			 * Die if the directory isn't writable.
			 */
			if (!is_writable(log_dir)) {
				fprintf(stderr, "--log-file: %s is not writable.\n", log_dir);
				exit(EXIT_FAILURE);
			}

			ftouch(&log_fp_err, log_path, GIT_STASHD_LOG_MODE);

			if (log_fp_err) {
				fprintf(stderr, "--log-file: Could not create %s\n", log_path);
				exit(EXIT_FAILURE);
			}
		}
	} else {
		/**
		 * Set absolute path to $HOME/git-stashd.log on log_path.
		 */
		copy(log_path, home_dir);
		concat(log_path, "/");
		concat(log_path, GIT_STASHD_LOG_FILE);

		/**
		 * Check if `log_path` is an existing file, or if it needs to be created.
		 */
		if (!is_file(log_path)) {

			/**
			 * Die if the directory isn't writable (for whatever odd, troublesome reason it could be).
			 */
			if (!is_writable(home_dir)) {
				fprintf(stderr, "--log-file: %s is not writable.\n", home_dir);
				exit(EXIT_FAILURE);
			}

			ftouch(&log_fp_err, log_path, GIT_STASHD_LOG_MODE);

			if (log_fp_err) {
				fprintf(stderr, "--log-file: Could not create %s\n", log_path);
				exit(EXIT_FAILURE);
			}
		}
	}

	/**
	 * Check if `--max-entries` option was given.
	 * If so, verify it's an integer argument.
	 */
	if (in_array(GIT_STASHD_OPT_MAX_ENTRIES_L, argv, argc) ||
	    in_array(GIT_STASHD_OPT_MAX_ENTRIES_S, argv, argc)) {

		opt_index = (index_of(GIT_STASHD_OPT_MAX_ENTRIES_L, argv, argc) != NOT_FOUND)
		          ? index_of(GIT_STASHD_OPT_MAX_ENTRIES_L, argv, argc)
		          : index_of(GIT_STASHD_OPT_MAX_ENTRIES_S, argv, argc);

		if ((arg_index = (opt_index + 1)) > last_index) {
			fprintf(stderr, "--max-entries: Missing argument\n");
			exit(EXIT_FAILURE);
		}

		copy(s_max_entries, argv[arg_index]);

		/**
		 * Verify `--max-entries` option argument is a valid number.
		 */
		if (!is_numeric(s_max_entries)) {
			fprintf(stderr, "--max-entries: Invalid argument %s. Argument must be an integer.\n", s_max_entries);
			exit(EXIT_FAILURE);
		}

		max_entries = atoi(s_max_entries);
	} else {
		max_entries = GIT_STASHD_MAX_ENTRIES;
	}

	/**
	 * Check if `--path` option was given. If so,
	 * get the absolute path of the pathname given.
	 */
	if (in_array(GIT_STASHD_OPT_PATH_L, argv, argc) ||
	    in_array(GIT_STASHD_OPT_PATH_S, argv, argc)) {

		opt_index = (index_of(GIT_STASHD_OPT_PATH_L, argv, argc) != NOT_FOUND)
		          ? index_of(GIT_STASHD_OPT_PATH_L, argv, argc)
		          : index_of(GIT_STASHD_OPT_PATH_S, argv, argc);

		if ((arg_index = (opt_index + 1)) > last_index) {
			fprintf(stderr, "--path: Missing pathname argument\n");
			exit(EXIT_FAILURE);
		}

		/**
		 * To prevent `realpath` from segfaulting when an invalid or malformed
		 * pathname is given, verify `argv[arg_index]` is a valid directory.
		 */
		if (!is_dir(argv[arg_index])) {
			fprintf(stderr, "--path: Invalid path %s\n", argv[arg_index]);
			exit(EXIT_FAILURE);
		}

		path = realpath(argv[arg_index], path_buf);

		if (IS_NULL(path)) {
			fprintf(stderr, "--path: Unable to access %s\n", argv[arg_index]);
			exit(EXIT_FAILURE);
		}
	} else {
		/**
		 * Since `--path` wasn't given, attempt
		 * to get the absolute path via `cwd`.
		 */
		path = getcwd(path_buf, PATH_MAX);

		if (IS_NULL(path)) {
			fprintf(stderr, "--path: Unable to get current working directory\n");
			exit(EXIT_FAILURE);
		}
	}

	/**
	 * Open Git repository, die if an error was encountered trying to do so.
	 */
	if (git_repository_open(&repo, path)) {
		fprintf(stderr, "Unable to locate repository at %s. Please verify it is a Git repository and you have sufficient permissions.\n", path);
		exit(EXIT_FAILURE);
	}

	/**
	 * If the repository already has a stashd.lock file,
	 * write a message to the log file and exit failure.
	 */
	if (has_lock(&lock_err, path)) {
		char *lock_err_msg, *lock_err_fmt = "Unable to create lock file in %s. File exists.";

		lock_err_msg = ALLOC(
			sizeof(char) * (
				(length(lock_err_fmt) + NUL_BYTE) +
				(length(path) + NUL_BYTE)
			)
		);
		sprintf(lock_err_msg, lock_err_fmt, path);

		flog(lock_err_msg);
		FREE(lock_err_msg);

		exit(EXIT_FAILURE);
	}

	/**
	 * Get absolute path to repository .git directory.
	 */
	copy(git_dir, get_git_dir(&git_dir_err, path));

	/**
	 * Assemble absolute path to stashd.lock file.
	 */
	copy(lock_file, git_dir);
	concat(lock_file, "/");
	concat(lock_file, GIT_STASHD_LOCK_FILE);

	/**
	 * Create stashd.lock file.
	 */
	ftouch(&lock_fp_err, lock_file, GIT_STASHD_LOCK_MODE);

	/**
	 * Daemonize, unless user explicitly gave --foreground option.
	 */
	if (daemonized) {
		daemonize();
	}

	/**
	 *
	 * Start initialization of stash and entries.
	 *
	 */
	stash_length = 0;

	/**
	 * Get initial stash length.
	 */
	git_stash_foreach(repo, init_setup, &stash_length);

	/**
	 * Allocate space for stash, repo structs.
	 */
	stash = ALLOC(sizeof(*stash));
	stash->repository = ALLOC(sizeof(*stash_repo));
	stash->length = (size_t) stash_length;

	copy(stash->repository->path, path);

	for (index = 0; index < stash->length; index += 1) {
		stash->entries[index] = ALLOC(sizeof(struct git_stashd_entry));
		stash->entries[index]->stash = stash;
	}

	/**
	 * Initialize stash entries on stash struct.
	 */
	git_stash_foreach(repo, init_stash, stash);

	/**
	 * Setup signal handling.
	 */
	action.sa_handler = &on_signal;
	action.sa_flags = SA_RESTART;
	sigfillset(&action.sa_mask);

	if (is_error(sigaction(SIGHUP, &action, NULL))) {
		perror("Error handling SIGHUP\n");
	}

	if (is_error(sigaction(SIGINT, &action, NULL))) {
		perror("Error handling SIGINT\n");
	}

	if (is_error(sigaction(SIGUSR1, &action, NULL))) {
		perror("Error handling SIGUSR1\n");
	}

	if (is_error(sigaction(SIGUSR2, &action, NULL))) {
		perror("Error handling SIGUSR2\n");
	}

	/**
	 * @todo: Add better cleanup for signal, error handlers.
	 */

	while (1) {
		/**
		 * Exit if stash length meets or exceeds our max_entries value.
		 */
		if (stash->length >= max_entries) {
			char *max_ent_info_msg,
			     *max_ent_info_fmt = "Reached max entries of %d in %s, exiting...";

			max_ent_info_msg = ALLOC(
				sizeof(char) * (
					(length(max_ent_info_fmt) + NUL_BYTE) +
					(sizeof(int) + NUL_BYTE) +
					(length(path) + NUL_BYTE)
				)
			);
			sprintf(
				max_ent_info_msg,
				max_ent_info_fmt,
				max_entries,
				path
			);

			/**
			 * Output max entries message to log file.
			 */
			flog(max_ent_info_msg);
			FREE(max_ent_info_msg);

			break;
		}

		int ds_err, wt_err, has_entry;
		char *log_info_msg,
		     /**
			  * @todo: Move this to a macro.
			  */
		     *log_info_fmt = "Checking worktree %s @ %s",
		     ts_buf[GIT_STASHD_TMS_LENGTH_MAX];

		/**
		 * Get timestamp for logging.
		 */
		get_timestamp(ts_buf);

		log_info_msg = ALLOC(
			sizeof(char) * (
				(length(log_info_fmt) + NUL_BYTE) +
				(length(path) + NUL_BYTE) +
				(length(ts_buf) + NUL_BYTE)
			)
		);
		sprintf(
			log_info_msg,
			log_info_fmt,
			path,
			ts_buf
		);

		/**
		 * Write informational message to log file.
		 */
		flog(log_info_msg);
		FREE(log_info_msg);

		/**
		 * Get the current index status from the worktree.
		 */
		index_status = is_worktree_dirty(&wt_err, path);

		if (wt_err) {
			char *wt_err_msg;

			wt_err_msg = ALLOC(
				sizeof(char) * (
					length(GIT_STASHD_CHECK_INDEX_STATUS_ERROR) + NUL_BYTE
				)
			);
			sprintf(
				wt_err_msg,
				GIT_STASHD_CHECK_INDEX_STATUS_ERROR
			);

			flog(wt_err_msg);
			FREE(wt_err_msg);

			exit(EXIT_FAILURE);
		}

		/**
		 * Check the stash for an existing entry
		 * matching the current worktree diff.
		 */
		entry_status = has_coequal_entry(
			&ds_err,
			path,
			stash
		);

		if (ds_err) {
			char *ds_err_msg;

			ds_err_msg = ALLOC(
				sizeof(char) * (
					length(GIT_STASHD_SEARCH_EQUIV_ENTRY_ERROR) + NUL_BYTE
				)
			);
			sprintf(
				ds_err_msg,
				GIT_STASHD_SEARCH_EQUIV_ENTRY_ERROR
			);

			flog(ds_err_msg);
			FREE(ds_err_msg);

			exit(EXIT_FAILURE);
		}

		/**
		 * @note: Use of is_error here might be a bit of an abuse.
		 *        Consider refactoring this to be more explicit.
		 */
		has_entry = !is_error(entry_status);

		/**
		 * If the worktree is dirty.
		 */
		if (index_status) {
			/**
			 * If there's no equivalent stash entry,
			 * create an entry and add it to the stash.
			 */
			if (!has_entry) {
				int ae_err;
				char *ae_info_msg;

				add_stash_entry(&ae_err, path, stash);

				if (ae_err) {
					flog(GIT_STASHD_ADD_ENTRY_TO_STASH_ERROR);
					exit(EXIT_FAILURE);
				}

				ae_info_msg = ALLOC(
					sizeof(char) * (
						(length(GIT_STASHD_WORKTREE_DIRTY_NEW_ENTRY) + NUL_BYTE)
					)
				);
				sprintf(
					ae_info_msg,
					GIT_STASHD_WORKTREE_DIRTY_NEW_ENTRY
				);

				flog(ae_info_msg);
				FREE(ae_info_msg);

				/**
				 * Update stash length.
				 */
				stash->length++;
			} else {
				char *ee_err_msg,
				     *ee_err_fmt = "--> Worktree is dirty, found equivalent entry at stash@{%d}. Skipping...";

				ee_err_msg = ALLOC(
					sizeof(char) * (
						(length(ee_err_fmt) + NUL_BYTE) +
						(sizeof(int) + NUL_BYTE)
					)
				);
				sprintf(
					ee_err_msg,
					ee_err_fmt,
					entry_status
				);

				flog(ee_err_msg);
				FREE(ee_err_msg);
			}
		} else {
			flog(GIT_STASHD_WORKTREE_CLEAN_NO_ACTION);
		}

		/**
		 * Wait `interval` seconds before continuing the loop.
		 */
		nap(interval);
	}

	/**
	 * @todo: Consolidate this in on_cleanup goto.
	 */
	unlink(lock_file);

	/**
	 * Clean up before exiting.
	 */
	for (index = 0; index < stash->length; index += 1) {
		FREE(stash->entries[index]);
	}

	FREE(stash->repository);
	FREE(stash);
	git_repository_free(repo);

	return EXIT_SUCCESS;
}
