/**
 * main.c
 *
 * Copyright (C) 2017 Nickolas Burr <nickolasburr@gmail.com>
 */

#include "main.h"

int main (int argc, char *argv[]) {
	int opt_index, arg_index, daemonize;
	char cwd[PATH_MAX], *pathname;
	struct repo *repo;
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
	repo->path = ALLOC(sizeof(char) * (strlen(pathname) + 1));
	repo->stash = ALLOC(sizeof(*stash));
	repo->stash->entries = ALLOC(sizeof(char) * 4096);

	// Copy `pathname` into repo struct `path` member.
	copy(repo->path, pathname);

	// Set stash entries on repo struct.
	set_stash(repo);

	printf("main -> repo->stash->entries -> \n%s\n", repo->stash->entries);

	printf("main -> is_worktree_dirty(repo) -> %d\n", is_worktree_dirty(repo));

	FREE(repo->stash->entries);
	FREE(repo->stash);
	FREE(repo->path);
	FREE(repo);

	if (daemonize) {
		fork_proc();

		write_log_file(GIT_STASHD_LOG_FILE, GIT_STASHD_LOG_MODE);
	}

//	printf("My PID is: %d\n", getpid());
//
//	action.sa_handler = &on_signal;
//	action.sa_flags = SA_RESTART;
//	sigfillset(&action.sa_mask);
//
//	if (sigaction(SIGHUP, &action, NULL) == -1) {
//		perror("ERROR: Cannot handle SIGHUP!");
//	}
//
//	if (sigaction(SIGUSR1, &action, NULL) == -1) {
//		perror("ERROR: Cannot handle SIGUSR1!");
//	}
//
//	if (sigaction(SIGKILL, &action, NULL) == -1) {
//		perror("ERROR: Cannot (and will never be able to) handle SIGKILL!");
//	}
//
//	if (sigaction(SIGINT, &action, NULL) == -1) {
//		perror("ERROR: Cannot handle SIGINT!");
//	}
//
//	while (1) {
//		printf("\nNapping for ~5 seconds\n");
//		nap(5);
//	}

	return EXIT_SUCCESS;
}
