/**
 * main.c
 *
 * Copyright (C) 2017 Nickolas Burr <nickolasburr@gmail.com>
 */

#include "argv.h"
#include "common.h"
#include "daemon.h"

int main (int argc, char *argv[]) {
	FILE *fp;
	int opt_index, arg_index, daemonize;
	long pid;
	char *pathname;
	struct repo_info *repo_info;

	/**
	 * If the `--help` option was given, display usage details and exit.
	 */
	if (opt_in_array(GIT_STASHD_OPT_HELP_L, argv, argc) ||
	    opt_in_array(GIT_STASHD_OPT_HELP_S, argv, argc)) {
		pfusage();

		exit(EXIT_SUCCESS);
	}

	/**
	 *
	 * If the `--daemon` (`-D`) option was given:
	 *
	 * 1) Look for a `--repository-path` (`-R`) option before starting a daemon. If it was given,
	 *    validate the option argument is a path to a Git repository.
	 * 2) If no `--repository-path` option was specified, check the CWD. Alas, if the CWD
	 *    is not a Git repository, emit an error and die.
	 *
	 */
	if (opt_in_array(GIT_STASHD_OPT_DAEMON_L, argv, argc) ||
	    opt_in_array(GIT_STASHD_OPT_DAEMON_S, argv, argc)) {
		daemonize = 1;
	}

	// Check for `--repository-path` option and argument
	if (opt_in_array(GIT_STASHD_OPT_REPOPATH_L, argv, argc) ||
	    opt_in_array(GIT_STASHD_OPT_REPOPATH_S, argv, argc)) {

		// Index of `--repository-path` option in `argv`
		opt_index = (opt_get_index(GIT_STASHD_OPT_REPOPATH_L, argv, argc) != -1)
		          ? opt_get_index(GIT_STASHD_OPT_REPOPATH_L, argv, argc)
		          : opt_get_index(GIT_STASHD_OPT_REPOPATH_S, argv, argc);

		// Index of `--repository-path` option argument in `argv`
		arg_index = (opt_index + 1);

		// Actual pathname string given as the option argument
		pathname  = argv[arg_index];

		// Die if `pathname` is not a valid directory
		if (!is_dir(pathname)) {
			printf("%s is not a valid directory!\n", pathname);

			exit(EXIT_FAILURE);
		}

		repo_info = (struct repo_info *) malloc(sizeof(struct repo_info));
		fp = get_log_file(GIT_STASHD_LOG_FILE, GIT_STASHD_LOG_MODE);

		repo_info->path = pathname;
		fprintf(fp, "main -> pathname -> %s\n", pathname);

		// start_daemon(pathname, &pid);
		// write_log_file(GIT_STASHD_LOG_FILE, GIT_STASHD_LOG_MODE);

		repo_info->path = pathname;
		repo_info->pid  = pid;

		// fprintf(fp, "main -> pid  -> %ld\n", pid);

		free(repo_info);
		fclose(fp);
	}

	if (daemonize) {
		printf("%d\n", daemonize);
	}

	return EXIT_SUCCESS;
}
