/**
 * main.c
 *
 * Copyright (C) 2017 Nickolas Burr <nickolasburr@gmail.com>
 */

#include "main.h"

/**
 * @notes extern log_path declared in common.h.
 */
char log_path[PATH_MAX];

int main (int argc, char **argv) {
	int index,
	    fp_err,
	    log_fp_err,
	    log_rw_err,
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
	 * If the `--help` option was given, display usage details and exit.
	 */
	if (in_array(GIT_STASHD_OPT_HELP_L, argv, argc) ||
	    in_array(GIT_STASHD_OPT_HELP_S, argv, argc)) {
		usage();
	}

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

			touch_log_file(&log_fp_err, log_path, GIT_STASHD_LOG_MODE);

			if (log_fp_err) {
				fprintf(stderr, "--log-file: Could not create %s\n", log_path);

				exit(EXIT_FAILURE);
			}
		}
	} else {
		/**
		 * Set realpath value on log_path.
		 */
		realpath(GIT_STASHD_LOG_FILE, log_path);
		copy(log_dir, dir_name(base_name(log_path)));

		/**
		 * Check if `log_path` is an existing file, or if it needs to be created.
		 */
		if (!is_file(log_path)) {

			/**
			 * Die if the directory isn't writable (for some odd reason).
			 */
			if (!is_writable(log_dir)) {
				fprintf(stderr, "--log-file: %s is not writable.\n", log_dir);

				exit(EXIT_FAILURE);
			}

			touch_log_file(&log_fp_err, log_path, GIT_STASHD_LOG_MODE);

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

		if (is_null(path)) {
			fprintf(stderr, "--path: Unable to access %s\n", argv[arg_index]);

			exit(EXIT_FAILURE);
		}
	} else {
		/**
		 * Since `--path` wasn't given, attempt
		 * to get the absolute path via `cwd`.
		 */
		path = getcwd(path_buf, PATH_MAX);

		if (is_null(path)) {
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
	 * Allocate space for stash and repo structs.
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
	 * @todo: Add better cleanup handling between main and signal handlers.
	 */

	while (1) {
		/**
		 * Exit if stash length meets or exceeds our max_entries value.
		 */
		if (stash->length >= max_entries) {
			char *max_ent_info_msg,
			     *max_ent_info_fmt = "Reached max entries of %d in %s, exiting...";

			max_ent_info_msg = ALLOC(sizeof(char) * ((strlen(max_ent_info_fmt) + NULL_BYTE) + (sizeof(int) + NULL_BYTE) + (strlen(path) + NULL_BYTE)));
			sprintf(max_ent_info_msg, max_ent_info_fmt, max_entries, path);

			/**
			 * Output max entries message to log file.
			 */
			write_to_log(&log_rw_err, log_path, GIT_STASHD_LOG_MODE, max_ent_info_msg);
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

		log_info_msg = ALLOC(sizeof(char) * ((strlen(log_info_fmt) + NULL_BYTE) + (strlen(path) + NULL_BYTE) + (strlen(ts_buf) + NULL_BYTE)));
		sprintf(log_info_msg, log_info_fmt, path, ts_buf);

		/**
		 * Write informational message to log file.
		 */
		write_to_log(&log_rw_err, log_path, GIT_STASHD_LOG_MODE, log_info_msg);
		FREE(log_info_msg);

		/**
		 * Get the current index status from the worktree.
		 */
		index_status = is_worktree_dirty(&wt_err, path);

		if (wt_err) {
			char *wt_err_msg;

			wt_err_msg = ALLOC(sizeof(char) * ((strlen(GIT_STASHD_CHECK_INDEX_STATUS_ERROR) + NULL_BYTE)));
			sprintf(wt_err_msg, GIT_STASHD_CHECK_INDEX_STATUS_ERROR);

			write_to_log(&log_rw_err, log_path, GIT_STASHD_LOG_MODE, wt_err_msg);
			FREE(wt_err_msg);

			exit(EXIT_FAILURE);
		}

		/**
		 * Check the stash for an existing entry
		 * matching the current worktree diff.
		 */
		entry_status = has_coequal_entry(&ds_err, path, stash);

		if (ds_err) {
			char *ds_err_msg;

			ds_err_msg = ALLOC(sizeof(char) * (strlen(GIT_STASHD_SEARCH_EQUIV_ENTRY_ERROR)));
			sprintf(ds_err_msg, GIT_STASHD_SEARCH_EQUIV_ENTRY_ERROR);

			write_to_log(&log_rw_err, log_path, GIT_STASHD_LOG_MODE, ds_err_msg);
			FREE(ds_err_msg);

			exit(EXIT_FAILURE);
		}

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
					write_to_log(&log_rw_err, log_path, GIT_STASHD_LOG_MODE, GIT_STASHD_ADD_ENTRY_TO_STASH_ERROR);

					exit(EXIT_FAILURE);
				}

				ae_info_msg = ALLOC(sizeof(char) * ((strlen(GIT_STASHD_WORKTREE_DIRTY_NEW_ENTRY) + NULL_BYTE)));
				sprintf(ae_info_msg, GIT_STASHD_WORKTREE_DIRTY_NEW_ENTRY);

				write_to_log(&log_rw_err, log_path, GIT_STASHD_LOG_MODE, ae_info_msg);
				FREE(ae_info_msg);

				/**
				 * Update stash length.
				 */
				stash->length++;
			} else {
				char *ee_err_msg,
				     *ee_err_fmt = "--> Worktree is dirty, found equivalent entry at stash@{%d}. Not adding duplicate entry.";

				ee_err_msg = ALLOC(sizeof(char) * ((strlen(ee_err_fmt) + NULL_BYTE) + (sizeof(int) + 1)));
				sprintf(ee_err_msg, ee_err_fmt, entry_status);

				write_to_log(&log_rw_err, log_path, GIT_STASHD_LOG_MODE, ee_err_msg);
				FREE(ee_err_msg);
			}
		} else {
			write_to_log(&log_rw_err, log_path, GIT_STASHD_LOG_MODE, GIT_STASHD_WORKTREE_CLEAN_NO_ACTION);
		}

		/**
		 * Wait `interval` seconds before continuing the loop.
		 */
		nap(interval);
	}

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
