/**
 * main.c
 *
 * Copyright (C) 2017 Nickolas Burr <nickolasburr@gmail.com>
 */

#include "main.h"

int main (int argc, char *argv[]) {
	int index,
	    fp_err,
	    fork_err,
	    init_err,
	    opt_index,
	    daemonize,
	    term_sig;
	char cwd[PATH_MAX], *path;
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
	 * Check if `--daemon` option was given. This will determine
	 * if a child process should be forked from the parent process.
	 */
	if (opt_in_array(GIT_STASHD_OPT_DAEMON_L, argv, argc) ||
	    opt_in_array(GIT_STASHD_OPT_DAEMON_S, argv, argc)) {
		daemonize = 1;
	}

	/**
	 * Check if `--repository-path` option was given. If so,
	 * check if a path was given as an option argument. If not,
	 * attempt to use `cwd` as the --repository-path pathname.
	 */
	if (opt_in_array(GIT_STASHD_OPT_REPOPATH_L, argv, argc) ||
	    opt_in_array(GIT_STASHD_OPT_REPOPATH_S, argv, argc)) {

		// Index of `--repository-path` option in `argv`
		opt_index = (opt_get_index(GIT_STASHD_OPT_REPOPATH_L, argv, argc) != NOT_FOUND)
		          ? opt_get_index(GIT_STASHD_OPT_REPOPATH_L, argv, argc)
		          : opt_get_index(GIT_STASHD_OPT_REPOPATH_S, argv, argc);

		// Actual path string given as the option argument
		path = argv[(opt_index + 1)];

		// printf("--repository-path option given, path set to -> %s\n", path);
	} else {
		/**
		 * Since `--repository-path` wasn't given,
		 * attempt to get the path from `cwd`.
		 */
		path = getcwd(cwd, sizeof(cwd));

		if (!path) {
			printf("Unable to get the current working directory!\n");

			exit(EXIT_FAILURE);
		}

		// printf("--repository-path option not given, assuming current path -> %s\n", path);
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

	if (daemonize) {
		fork_proc(&fork_err);
	}

	/**
	 * Exit failure, if an error was encountered forking the parent process.
	 */
	if (fork_err) {
		printf("An error was encountered when trying to fork the main process.\n");

		exit(EXIT_FAILURE);
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
		printf("An error was encountered while trying to retrieve stash entries for %s\n", path);

		exit(EXIT_FAILURE);
	}

	/**
	 * Setup signal handling.
	 */
	printf("PID: %d\n", getpid());

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

	while (1) {
		printf("\nNapping for ~5 seconds.\n");
		nap(5);
	}

	printf("PID %d terminated.\n", getpid());

	for (index = 0; index < GIT_STASHD_ENT_LENGTH_MAX; index += 1) {
		FREE(repo->stash->entries[index]);
	}

	FREE(repo->stash);
	FREE(repo);

	return EXIT_SUCCESS;
}
