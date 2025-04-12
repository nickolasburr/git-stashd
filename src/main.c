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
	int index;
	int git_dir_err, lock_err,
	    lock_fp_err, log_fp_err,
	    repo_err;
	int optval, loptindex;
	int foreground, interval;
	int entry_status, index_status,
	    last_index, max_entries,
	    stash_length;
	int log_file_init, git_path_init;
	char pathbuf[PATH_MAX],
	     gitpath[PATH_MAX],
	     git_dir[PATH_MAX],
	     log_dir[PATH_MAX],
	     intvbuf[5],
	     maxnbuf[5];
	char *cwd = NULL,
		 *homedir = NULL,
	     *logfile = NULL,
	     *path = NULL;

	git_repository *repo = NULL;
	struct git_stashd_stash *stash = NULL;
	struct git_stashd_repository *stash_repo = NULL;
	struct sigaction action;

	/**
	 * OPTIONS
	 *
	 * -F, --foreground             Run daemon in foreground. Helpful for debugging.
	 * -i, --interval=<NUM>         Interval (in seconds) to check for changes. Defaults to 600 (10 minutes).
	 * -L, --log-file=<PATH>        Path to alternate log file. Default location is $HOME/git-stashd.log.
	 * -M, --max-entries=<NUM>      Maximum number of entries a stash should hold. Defaults to 100.
	 *                              When --max-entries threshold is met, the daemon will exit.
	 * -p, --path=<PATH>            Path to Git repository. Defaults to $PWD.
	 * -h, --help                   Show help information and usage examples.
	 * -v, --version                Show version information.
	 */

	static struct option longopts[] = {
		{ "foreground", no_argument, NULL, 'F' },
		{ "interval", optional_argument, NULL, 'i' },
		{ "log-file", optional_argument, NULL, 'L' },
		{ "max-entries", optional_argument, 0, 'M' },
		{ "path", optional_argument, 0, 'p' },
		{ "help", no_argument, 0, 'h' },
		{ "version", no_argument, 0, 'v' },
	};

	git_path_init = 0;
	log_file_init = 0;

	foreground = 0;
	max_entries = GIT_STASHD_MAX_ENTRIES;
	interval = GIT_STASHD_INTERVAL_DEFAULT_SEC;

	do {
		optval = getopt_long(
			argc,
			argv,
			"Fi:L:M:p:hv",
			longopts,
			&loptindex
		);

		if (optval == -1) {
			break;
		}

		switch (optval) {
			case 'F':
				foreground = 1;
				break;
			case 'i':
				strcpy(intvbuf, optarg);

				if (!is_numeric(intvbuf)) {
					fprintf(
						stderr,
						"--interval: Invalid argument '%s', must be an integer.\n",
						intvbuf
					);
					exit(EXIT_FAILURE);
				}

				interval = (int) strtoul(intvbuf, NULL, 0);

				if (interval < GIT_STASHD_INTERVAL_LOWER_BOUND) {
					fprintf(
						stderr,
						"--interval: To prevent overload, interval must be >= %d.\n",
						GIT_STASHD_INTERVAL_LOWER_BOUND
					);
					exit(EXIT_FAILURE);
				}

				break;
			case 'L':
				realpath(optarg, log_path);
				logfile = base_name(log_path);
				strcpy(
					log_dir,
					dir_name(logfile)
				);

				if (!is_file(log_path)) {
					if (!is_writable(log_dir)) {
						fprintf(
							stderr,
							"--log-file: %s is not writable.\n",
							log_dir
						);
						exit(EXIT_FAILURE);
					}

					ftouch(
						&log_fp_err,
						log_path,
						GIT_STASHD_LOG_MODE
					);

					if (log_fp_err) {
						fprintf(
							stderr,
							"--log-file: Could not create log file %s\n",
							log_path
						);
						exit(EXIT_FAILURE);
					}
				}

				log_file_init = 1;
				break;
			case 'M':
				strcpy(maxnbuf, optarg);

				if (!is_numeric(maxnbuf)) {
					fprintf(
						stderr,
						"--max-entries: Invalid argument %s. Argument must be an integer.\n",
						maxnbuf
					);
					exit(EXIT_FAILURE);
				}

				max_entries = (int) strtoul(maxnbuf, NULL, 0);
				break;
			case 'p':
				strcpy(pathbuf, optarg);

				/**
				 * To prevent `realpath` from segfaulting when an invalid or malformed
				 * pathname is given, verify `pathbuf` is a valid directory.
				 */
				if (!is_dir(pathbuf)) {
					fprintf(
						stderr,
						"--path: Invalid path %s\n",
						pathbuf
					);
					exit(EXIT_FAILURE);
				}

				path = realpath(
					pathbuf,
					gitpath
				);

				if (path == NULL) {
					fprintf(
						stderr,
						"--path: Unable to access %s\n",
						pathbuf
					);
					exit(EXIT_FAILURE);
				}

				git_path_init = 1;
				break;
			case 'h':
				usage();
				exit(EXIT_SUCCESS);
			case 'v':
				fprintf(
					stdout,
					"%s\n",
					GIT_STASHD_VERSION
				);
				exit(EXIT_SUCCESS);
			case '?':
				usage();
				exit(EXIT_FAILURE);
			default:
				break;
		}
	} while (1);

	git_libgit2_init();

	if (!log_file_init) {
		homedir = getenv("HOME");
		strcpy(log_path, homedir);
		strcat(log_path, "/");
		strcat(
			log_path,
			GIT_STASHD_LOG_FILE
		);

		if (!is_file(log_path)) {
			if (!is_writable(homedir)) {
				fprintf(
					stderr,
					"--log-file: %s is not writable.\n",
					homedir
				);
				exit(EXIT_FAILURE);
			}

			ftouch(
				&log_fp_err,
				log_path,
				GIT_STASHD_LOG_MODE
			);

			if (log_fp_err) {
				fprintf(
					stderr,
					"--log-file: Could not create log file %s\n",
					log_path
				);
				exit(EXIT_FAILURE);
			}
		}
	}

	if (!git_path_init) {
		path = getcwd(pathbuf, PATH_MAX);

		if (path == NULL) {
			fprintf(
				stderr,
				"--path: Unable to get current working directory\n"
			);
			exit(EXIT_FAILURE);
		}
	}

	repo_err = git_repository_open(
		&repo,
		path
	);

	if (repo_err) {
		fprintf(
			stderr,
			GIT_STASHD_CANNOT_LOCATE_REPO_ERROR,
			path
		);
		exit(EXIT_FAILURE);
	}

	/**
	 * If the repository already has a stashd.lock file,
	 * write a message to the log file and exit failure.
	 */
	if (has_lock(&lock_err, path)) {
		char *lock_err_msg = ALLOC(
			sizeof(char) * (
				(strlen(GIT_STASHD_GENERATE_LOCK_FILE_ERROR) + NUL_BYTE) +
				(strlen(path) + NUL_BYTE)
			)
		);
		sprintf(
			lock_err_msg,
			GIT_STASHD_GENERATE_LOCK_FILE_ERROR,
			path
		);

		flog(lock_err_msg);
		FREE(lock_err_msg);

		exit(EXIT_FAILURE);
	}

	/**
	 * Get absolute path to repository .git directory.
	 */
	strcpy(
		git_dir,
		get_git_dir(&git_dir_err, path)
	);

	strcpy(lock_file, git_dir);
	strcat(lock_file, "/");
	strcat(
		lock_file,
		GIT_STASHD_LOCK_FILE
	);
	ftouch(
		&lock_fp_err,
		lock_file,
		GIT_STASHD_LOCK_MODE
	);

	if (lock_fp_err) {
		fprintf(
			stderr,
			"Unable to create lock file %s\n",
			lock_file
		);
		exit(EXIT_FAILURE);
	}

	if (!foreground) {
		daemonize();
	}

	stash_length = 0;
	git_stash_foreach(
		repo,
		init_setup,
		&stash_length
	);

	stash = ALLOC(sizeof(*stash));
	stash->repository = ALLOC(sizeof(*stash_repo));
	stash->length = (size_t) stash_length;

	strcpy(
		stash->repository->path,
		path
	);

	for (index = 0; index < stash->length; index += 1) {
		stash->entries[index] = ALLOC(
			sizeof(struct git_stashd_entry)
		);
		stash->entries[index]->stash = stash;
	}

	git_stash_foreach(
		repo,
		init_stash,
		stash
	);

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
	 * @todo: Add better cleanup for signal, error handlers.
	 */

	do {
		if (stash->length >= max_entries) {
			char *max_ent_info_msg = ALLOC(
				sizeof(char) * (
					(strlen(GIT_STASHD_STASH_STATUS_MAX_ENTRIES) + NUL_BYTE) +
					(sizeof(int) + NUL_BYTE) +
					(strlen(path) + NUL_BYTE)
				)
			);
			sprintf(
				max_ent_info_msg,
				GIT_STASHD_STASH_STATUS_MAX_ENTRIES,
				max_entries,
				path
			);

			flog(max_ent_info_msg);
			FREE(max_ent_info_msg);

			break;
		}

		int ds_err, wt_err;
		char *log_info_msg,
		     ts_buf[GIT_STASHD_TMS_LENGTH_MAX];

		get_timestamp(ts_buf);
		log_info_msg = ALLOC(
			sizeof(char) * (
				(strlen(GIT_STASHD_WORKTREE_CHECKING_STATUS) + NUL_BYTE) +
				(strlen(path) + NUL_BYTE) +
				(strlen(ts_buf) + NUL_BYTE)
			)
		);
		sprintf(
			log_info_msg,
			GIT_STASHD_WORKTREE_CHECKING_STATUS,
			path,
			ts_buf
		);

		flog(log_info_msg);
		FREE(log_info_msg);

		index_status = is_worktree_dirty(
			&wt_err,
			path
		);

		if (wt_err) {
			flog(GIT_STASHD_CHECK_INDEX_STATUS_ERROR);
			exit(EXIT_FAILURE);
		}

		/**
		 * Check the stash for an existing entry
		 * matching the current worktree diff.
		 */
		entry_status = has_match_entry(
			&ds_err,
			path,
			stash
		);

		if (ds_err) {
			flog(GIT_STASHD_SEARCH_EQUIV_ENTRY_ERROR);
			exit(EXIT_FAILURE);
		}

		/**
		 * If worktree is dirty.
		 */
		if (index_status) {
			/**
			 * If there's no equivalent stash entry,
			 * create an entry and add it to the stash.
			 */
			if (entry_status == -1) {
				int ae_err;

				add_stash_entry(
					&ae_err,
					path,
					stash
				);

				if (ae_err) {
					flog(GIT_STASHD_ADD_ENTRY_TO_STASH_ERROR);
					exit(EXIT_FAILURE);
				}

				flog(GIT_STASHD_WORKTREE_DIRTY_NEW_ENTRY);
				stash->length++;
			} else {
				char *ee_err_msg = ALLOC(
					sizeof(char) * (
						(strlen(GIT_STASHD_WORKTREE_DIRTY_HAS_MATCH) + NUL_BYTE) +
						(sizeof(int) + NUL_BYTE)
					)
				);
				sprintf(
					ee_err_msg,
					GIT_STASHD_WORKTREE_DIRTY_HAS_MATCH,
					entry_status
				);

				flog(ee_err_msg);
				FREE(ee_err_msg);
			}
		} else {
			flog(GIT_STASHD_WORKTREE_CLEAN_NO_ACTION);
		}

		nap(interval);
	} while (1);

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
