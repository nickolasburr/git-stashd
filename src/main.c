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
	    opt_index,
	    daemonize,
	    entry_status,
	    index_status,
	    interval;
	char path_buf[PATH_MAX],
	     s_interval[4],
	     *cwd,
	     *log_file,
	     *path;
	struct repository *repo;
	struct stash *stash;
	struct sigaction action;

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

		log_file = argv[(opt_index + 1)];
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

		copy(s_interval, argv[(opt_index + 1)]);

		printf("main -> s_interval -> %s\n", s_interval);

		/**
		 * Verify `--interval` option argument is a valid number.
		 */
		if (!is_numeric(s_interval)) {
			fprintf(stderr, "Interval given via --interval is not a valid number!\n");

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

		path = realpath(argv[(opt_index + 1)], path_buf);
	} else {
		/**
		 * Since `--path` wasn't given, attempt
		 * to get the absolute path via `cwd`.
		 */
		path = getcwd(path_buf, PATH_MAX);

		if (is_null(path)) {
			printf("Unable to get the current working directory!\n");

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
			fprintf(stderr, "Unable to create %s in %s\n", log_file);

			exit(EXIT_FAILURE);
		}
	}

	/**
	 * Validate path is an existing directory within reach.
	 */
	if (!is_dir(path)) {
		printf("%s is not a directory!\n", path);

		exit(EXIT_FAILURE);
	}

	/**
	 * And that it's also a Git repository.
	 */
	if (!is_repo(path)) {
		printf("%s is not a Git repository!\n", path);

		exit(EXIT_FAILURE);
	}

	/**
	 * Daemonize if the user requested it.
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
		int sd_err, wt_err;
		char *log_info_msg,
		     /**
			  * @todo: Move this to a macro.
			  */
		     *log_info_fmt = "git-stashd: Last check ran @ %s",
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
			char *wt_err_msg, wt_err_fmt = "Encountered an error when checking the index status. Status code %d\n";

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
		entry_status = has_coequal_entry(&sd_err, repo->stash);

		if (sd_err) {
			char *sd_err_msg, sd_err_fmt = "Encountered an error when searching for a matching entry. Status code %d\n";

			sd_err_msg = ALLOC(sizeof(char) * ((strlen(sd_err_fmt)) + (sizeof(int) + 1)));
			sprintf(sd_err_msg, sd_err_fmt, sd_err);

			write_log_file(&fp_err, GIT_STASHD_LOG_FILE, GIT_STASHD_LOG_MODE, sd_err_msg);

			FREE(sd_err_msg);

			exit(EXIT_FAILURE);
		}

		/**
		 * If the worktree is dirty and there's not an
		 * equivalent entry, create and add a new entry.
		 */
		if (index_status && !entry_status) {
			int ae_err;

			add_entry(&ae_err, repo->stash);

			if (ae_err) {
				char *log_err_msg, log_err_fmt = "Encountered an error when adding an entry to the stash. Status code %d\n";

				log_err_msg = ALLOC(sizeof(char) * ((strlen(log_err_fmt)) + (sizeof(int) + 1)));
				sprintf(log_err_msg, log_err_fmt, ae_err);

				write_log_file(&fp_err, GIT_STASHD_LOG_FILE, GIT_STASHD_LOG_MODE, log_err_msg);

				FREE(log_err_msg);

				exit(EXIT_FAILURE);
			}

			/**
			 * Update stash length.
			 */
			repo->stash->length++;
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
