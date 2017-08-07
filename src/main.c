/**
 * main.c
 *
 * Copyright (C) 2017 Nickolas Burr <nickolasburr@gmail.com>
 */

#include "common.h"
#include "argv.h"
#include "daemon.h"
#include "usage.h"

static struct {
	char *path;
	pid_t pid;
} repo_info;

int main (int argc, char *argv[]) {
	int opt_index, path_index;
	char *pathname;

	/**
	 * If the `--help` option was given, display usage details and exit.
	 */
	if (opt_in_array(GIT_STASHD_USAGE_OPT, argv, argc)) {
		pfusage();

		printf("%d\n", is_dir("/var/log"));

		exit(EXIT_SUCCESS);
	}

	/**
	 * If the `--daemon` option was given:
	 *
	 * 1) Look for a `--repository-path` option before starting a daemon. If it was given,
	 *    validate the option argument is a path to a Git repository.
	 * 2) If no `--repository-path` option was specified, check the CWD. Alas, if the CWD
	 *    is not a Git repository, emit an error and die.
	 */
	if (opt_in_array("--daemon", argv, argc)) {

		// Check for `--repository-path` option
		if (opt_in_array("--repository-path", argv, argc)) {

			// `--repository-path` index in `argv`
			opt_index  = opt_get_index("--repository-path", argv, argc);
			path_index = (opt_index + 1);
			pathname   = argv[path_index];

			// Die if `pathname` is not a valid directory
			if (!is_dir(pathname)) {
				printf("%s is not a valid directory!\n", pathname);

				exit(EXIT_FAILURE);
			}

			repo_info.path = pathname;
			repo_info.pid  = start_daemon(pathname);

			printf("%s\n", pathname);
		}
	}

	return EXIT_SUCCESS;
}
