/**
 * main.c
 *
 * Copyright (C) 2017 Nickolas Burr <nickolasburr@gmail.com>
 */

#include "common.h"
#include "argv.h"
#include "daemon.h"
#include "usage.h"

int main (int argc, char *argv[]) {
	FILE *fp;
	int oindex, pindex;
	long pid;
	char *path;
	struct repo_info *repo_info;

	/**
	 * If the `--help` option was given, display usage details and exit.
	 */
	if (opt_in_array(GIT_STASHD_USAGE_OPT, argv, argc)) {
		pfusage();

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
			oindex  = opt_get_index("--repository-path", argv, argc);
			pindex = (oindex + 1);
			path   = argv[pindex];

			// Die if `path` is not a valid directory
			if (!is_dir(path)) {
				printf("%s is not a valid directory!\n", path);

				exit(EXIT_FAILURE);
			} else {
				repo_info = (struct repo_info *) malloc(sizeof(struct repo_info));
				fp = get_log_file(GIT_STASHD_LOG_FILE, GIT_STASHD_LOG_MODE);

				repo_info->path = path;
				fprintf(fp, "main -> path -> %s\n", path);

				start_daemon(path, &pid);
				write_log_file(GIT_STASHD_LOG_FILE, GIT_STASHD_LOG_MODE);

				repo_info->path = path;
				repo_info->pid  = pid;

				fprintf(fp, "main -> pid  -> %ld\n", pid);

				free(repo_info);
				fclose(fp);
			}
		}
	}

	return EXIT_SUCCESS;
}
