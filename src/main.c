/**
 * main.c
 *
 * Copyright (C) 2017 Nickolas Burr <nickolasburr@gmail.com>
 */

#include "main.h"

int main (int argc, char *argv[]) {
	int i, j, opt_index, arg_index, daemonize;
	char cwd[PATH_MAX], *pathname;
	struct repository *repo;
	struct stash *stash;
	struct sigaction action;

	/**
	 * If the `--help` option was given, display usage details and exit.
	 */
	if (opt_in_array(GIT_STASHD_OPT_HELP_L, argv, argc) ||
	    opt_in_array(GIT_STASHD_OPT_HELP_S, argv, argc)) {
		pfusage();

		exit(EXIT_SUCCESS);
	}

	/**
	 * Check if `--daemon` (`-D`) option was given. This will determine
	 * if the process should detach from its controlling tty or not.
	 */
	if (opt_in_array(GIT_STASHD_OPT_DAEMON_L, argv, argc) ||
	    opt_in_array(GIT_STASHD_OPT_DAEMON_S, argv, argc)) {
		daemonize = 1;
	}

	/**
	 * Check if `--repository-path` (`-P`) option was given. If so,
	 * check if a pathname was given as an option argument. If not,
	 * attempt to use `cwd` as the --repository-path pathname.
	 */
	if (opt_in_array(GIT_STASHD_OPT_REPOPATH_L, argv, argc) ||
	    opt_in_array(GIT_STASHD_OPT_REPOPATH_S, argv, argc)) {

		// Index of `--repository-path` option in `argv`
		opt_index = (opt_get_index(GIT_STASHD_OPT_REPOPATH_L, argv, argc) != NOOPT_FOUND_V)
		          ? opt_get_index(GIT_STASHD_OPT_REPOPATH_L, argv, argc)
		          : opt_get_index(GIT_STASHD_OPT_REPOPATH_S, argv, argc);

		// Index of `--repository-path` option argument in `argv`
		arg_index = (opt_index + 1);

		// Actual pathname string given as the option argument
		pathname  = argv[arg_index];

		printf("--repository-path option given, pathname set to -> %s\n", pathname);
	} else {
		/**
		 * Since `--repository-path` wasn't given,
		 * attempt to get the pathname from `cwd`.
		 */
		pathname = getcwd(cwd, sizeof(cwd));

		if (!pathname) {
			printf("Unable to get the current working directory!\n");

			exit(EXIT_FAILURE);
		}

		printf("--repository-path option not given, assuming current pathname -> %s\n", pathname);
	}

	/**
	 * Validate pathname is an existing directory within reach.
	 */
	if (!is_dir(pathname)) {
		printf("%s is not a directory!\n", pathname);

		exit(EXIT_FAILURE);
	}

	/**
	 * And that it's also a Git repository.
	 */
	if (!is_repo(pathname)) {
		printf("%s is not a Git repository!\n", pathname);

		exit(EXIT_FAILURE);
	}

	/**
	 * Initialize struct for storing information
	 * specific to the Git repository in question.
	 */
	repo = ALLOC(sizeof(*repo));
	repo->stash = ALLOC(sizeof(*stash));

	// Copy `pathname` into repo struct `path` member.
	copy(repo->path, pathname);

	for (i = 0; i < 50; i += 1) {
		repo->stash->entries[i] = ALLOC(sizeof(struct entry));
	}

	// Set stash on repository struct.
	set_stash(repo);

	// List stash entries.
	list_entries(repo->stash);

	for (j = 0; j < 50; j += 1) {
		FREE(repo->stash->entries[j]);
	}

	FREE(repo->stash);
	FREE(repo);

	if (daemonize) {
		fork_proc();

		write_log_file(GIT_STASHD_LOG_FILE, GIT_STASHD_LOG_MODE);
	}

	return EXIT_SUCCESS;
}
