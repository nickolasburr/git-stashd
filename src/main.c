/**
 * main.c
 *
 * Copyright (C) 2017 Nickolas Burr <nickolasburr@gmail.com>
 */

#include "main.h"

int main (int argc, char **argv) {
	int index,
	    fp_err,
	    init_err,
	    arg_index,
	    opt_index,
	    daemonize,
	    entry_status,
	    index_status,
	    interval,
	    last_index;
	char path_buf[PATH_MAX],
	     s_interval[4],
	     *cwd,
	     *log_file,
	     *path;
	struct repository *repo;
	struct stash *stash;
	struct sigaction action;

	/**
	 * Get the index of the last element in `argv`.
	 */
	last_index = (argc - 1);

	/**
	 * Daemonize by default.
	 */
	daemonize = 1;

	/**
	 * If the `--help` option was given, display usage details and exit.
	 */
	if (in_array(GIT_STASHD_OPT_HELP_L, argv, argc) ||
	    in_array(GIT_STASHD_OPT_HELP_S, argv, argc)) {
		pfusage();

		exit(EXIT_SUCCESS);
	}

	/**
	 * Check if `--foreground` option was given. This will determine
	 * if a child process should be forked from the parent process.
	 */
	if (in_array(GIT_STASHD_OPT_FOREGROUND_L, argv, argc) ||
	    in_array(GIT_STASHD_OPT_FOREGROUND_S, argv, argc)) {
		daemonize = 0;
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
			fprintf(stderr, "Missing argument for --log-file\n");

			exit(EXIT_FAILURE);
		}

		log_file = argv[arg_index];
	} else {
		log_file = GIT_STASHD_LOG_FILE;
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
			fprintf(stderr, "Missing argument for --interval\n");

			exit(EXIT_FAILURE);
		}

		copy(s_interval, argv[arg_index]);

		/**
		 * Verify `--interval` option argument is a valid number.
		 */
		if (!is_numeric(s_interval)) {
			fprintf(stderr, "Invalid argument %s for --interval. Argument must be an integer.\n", s_interval);

			exit(EXIT_FAILURE);
		}

		interval = atoi(s_interval);
	} else {
		interval = GIT_STASHD_INTERVAL;
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
			fprintf(stderr, "Missing argument for --path\n");

			exit(EXIT_FAILURE);
		}

		/**
		 * To prevent `realpath` from segfaulting when an invalid or malformed
		 * pathname is given, verify `argv[arg_index]` is a valid directory.
		 */
		if (!is_dir(argv[arg_index])) {
			fprintf(stderr, "Invalid path %s for --path\n", argv[arg_index]);

			exit(EXIT_FAILURE);
		}

		path = realpath(argv[arg_index], path_buf);

		if (is_null(path)) {
			fprintf(stderr, "--path: Unable to access %s\n");

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
	 *
	 * @todo: Add check for --log-file, create log file if it doesn't already exist.
	 *
	 */

	if (!is_file(log_file)) {
		/**
		 * @todo: Check against the directory containing `--log-file` argument.
		 * For now, we'll use the default GIT_STASHD_LOG_DIR directory.
		 */
		if (!is_writable(GIT_STASHD_LOG_DIR)) {
			fprintf(stderr, "%s is not writable!\n", GIT_STASHD_LOG_DIR);

			exit(EXIT_FAILURE);
		}

		touch_log_file(&fp_err, log_file, GIT_STASHD_LOG_MODE);

		/**
		 * Exit failure, if a log file couldn't be created.
		 */
		if (fp_err) {
			fprintf(stderr, "Unable to create log file %s in %s\n", log_file, GIT_STASHD_LOG_DIR);

			exit(EXIT_FAILURE);
		}
	}

	/**
	 * Verify `path` is a Git repository.
	 */
	if (!is_repo(path)) {
		fprintf(stderr, "%s is not a Git repository\n", path);

		exit(EXIT_FAILURE);
	}

	/**
	 * Daemonize, unless user explicitly gave --foreground option.
	 */
	if (daemonize) {
		fork_proc();
	}

	/**
	 * Initialize struct for storing information
	 * specific to the Git repository in question.
	 */
	repo = ALLOC(sizeof(*repo));
	repo->stash = ALLOC(sizeof(*stash));
	repo->stash->repo = &repo;

	copy(repo->path, path);

	for (index = 0; index < GIT_STASHD_ENT_LENGTH_MAX; index += 1) {
		repo->stash->entries[index] = ALLOC(sizeof(struct entry));

		/**
		 * Set pointers to repo struct and stash struct.
		 */
		repo->stash->entries[index]->stash = repo->stash;
		repo->stash->entries[index]->stash->repo = repo;
	}

	/**
	 * Initialize stash.
	 */
	init_stash(&init_err, repo);

	/**
	 * Exit failure, if an error was encountered initializing the stash.
	 */
	if (init_err) {
		/**
		 * If we're daemonized, write the error to the log file.
		 * Otherwise, send it to STDERR.
		 */
		if (daemonize) {
			write_log_file(&fp_err, GIT_STASHD_LOG_FILE, GIT_STASHD_LOG_MODE, "An error was encountered while trying to retrieve stash entries...\n");
		} else {
			fprintf(stderr, "An error was encountered while trying to retrieve stash entries for %s\n", path);
		}

		exit(EXIT_FAILURE);
	}

	/**
	 * Setup signal handling.
	 */
	action.sa_handler = &on_signal;
	action.sa_flags = SA_RESTART;
	sigfillset(&action.sa_mask);

	if (sigaction(SIGHUP, &action, NULL) == -1) {
		perror("Error handling SIGHUP\n");
	}

	if (sigaction(SIGINT, &action, NULL) == -1) {
		perror("Error handling SIGINT\n");
	}

	if (sigaction(SIGUSR1, &action, NULL) == -1) {
		perror("Error handling SIGUSR1\n");
	}

	if (sigaction(SIGUSR2, &action, NULL) == -1) {
		perror("Error handling SIGUSR2\n");
	}

	/**
	 * @todo: Add better cleanup handling between main and signal handlers.
	 */

	while (1) {
		int ds_err, wt_err, has_entry;
		char *log_info_msg,
		     /**
			  * @todo: Move this to a macro.
			  */
		     *log_info_fmt = "Checked worktree @ %s",
		     ts_buf[GIT_STASHD_TMS_LENGTH_MAX];

		/**
		 * Get timestamp for logging.
		 */
		get_timestamp(ts_buf);

		log_info_msg = ALLOC(sizeof(char) * ((strlen(log_info_fmt) + NULL_BYTE) + (strlen(ts_buf) + NULL_BYTE)));
		sprintf(log_info_msg, log_info_fmt, ts_buf);

		/**
		 * Write informational message to log file.
		 */
		write_log_file(&fp_err, GIT_STASHD_LOG_FILE, GIT_STASHD_LOG_MODE, log_info_msg);
		FREE(log_info_msg);

		/**
		 * Get the current index status from the worktree.
		 */
		index_status = is_worktree_dirty(&wt_err, repo);

		if (wt_err) {
			char *wt_err_msg, wt_err_fmt = "Encountered an error when checking the index status. Status code %d";

			wt_err_msg = ALLOC(sizeof(char) * ((strlen(wt_err_fmt)) + (sizeof(int) + 1)));
			sprintf(wt_err_msg, wt_err_fmt, wt_err);

			write_log_file(&fp_err, GIT_STASHD_LOG_FILE, GIT_STASHD_LOG_MODE, wt_err_msg);
			FREE(wt_err_msg);

			exit(EXIT_FAILURE);
		}

		/**
		 * Check the stash for an existing entry
		 * matching the current worktree diff.
		 */
		entry_status = has_coequal_entry(&ds_err, repo->stash);

		if (ds_err) {
			char *ds_err_msg, ds_err_fmt = "Encountered an error when searching for a matching entry. Status code %d";

			ds_err_msg = ALLOC(sizeof(char) * ((strlen(ds_err_fmt)) + (sizeof(int) + 1)));
			sprintf(ds_err_msg, ds_err_fmt, ds_err);

			write_log_file(&fp_err, GIT_STASHD_LOG_FILE, GIT_STASHD_LOG_MODE, ds_err_msg);
			FREE(ds_err_msg);

			exit(EXIT_FAILURE);
		}

		has_entry = (entry_status != -1);

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

				add_entry(&ae_err, repo->stash);

				if (ae_err) {
					char *ae_err_msg, *ae_err_fmt = "Encountered an error when adding an entry to the stash";

					ae_err_msg = ALLOC(sizeof(char) * (strlen(ae_err_fmt) + NULL_BYTE));
					sprintf(ae_err_msg, ae_err_fmt);

					write_log_file(&fp_err, GIT_STASHD_LOG_FILE, GIT_STASHD_LOG_MODE, ae_err_msg);
					FREE(ae_err_msg);

					exit(EXIT_FAILURE);
				}

				/**
				 * Update stash length.
				 */
				repo->stash->length++;
			} else {
				char *ee_err_msg, *ee_err_fmt = "Worktree is dirty, but equivalent entry exists at stash@{%d}";

				ee_err_msg = ALLOC(sizeof(char) * ((strlen(ee_err_fmt) + NULL_BYTE) + (sizeof(int) + 1)));
				sprintf(ee_err_msg, ee_err_fmt, entry_status);

				write_log_file(&fp_err, GIT_STASHD_LOG_FILE, GIT_STASHD_LOG_MODE, ee_err_msg);
				FREE(ee_err_msg);
			}
		} else {
			char *clean_index_msg, *clean_index_fmt = "Worktree is clean, no action taken";

			clean_index_msg = ALLOC(sizeof(char) * (strlen(clean_index_fmt) + NULL_BYTE));

			write_log_file(&fp_err, GIT_STASHD_LOG_FILE, GIT_STASHD_LOG_MODE, clean_index_msg);
			FREE(clean_index_msg);
		}

		/**
		 * Wait `interval` seconds before continuing the loop.
		 */
		nap(interval);
	}

	/**
	 * Clean up before exiting.
	 */

	for (index = 0; index < GIT_STASHD_ENT_LENGTH_MAX; index += 1) {
		FREE(repo->stash->entries[index]);
	}

	FREE(repo->stash);
	FREE(repo);

	return EXIT_SUCCESS;
}
