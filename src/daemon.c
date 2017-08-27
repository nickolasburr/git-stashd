/**
 * daemon.c
 *
 * Copyright (C) 2017 Nickolas Burr <nickolasburr@gmail.com>
 */

#include "daemon.h"

/**
 * Fork parent process and run in detached daemon mode.
 */
void fork_proc (int *error) {
	int x;
	pid_t pid = fork();

	*error = 0;

	/**
	 * @todo: Refactor control flow, it should be much more concise.
	 */

	if (pid < 0) {
		*error = 1;

		return;
	}

	if (pid > 0) {
		return;
	}

	if (setsid() < 0) {
		*error = 1;

		return;
	}

	signal(SIGCHLD, SIG_IGN);
	signal(SIGHUP, SIG_IGN);

	pid = fork();

	if (pid < 0) {
		*error = 1;

		return;
	}

	if (pid > 0) {
		return;
	}

	umask(0);

	for (x = sysconf(_SC_OPEN_MAX); x >= 0; x--) {
		close(x);
	}
}

/**
 * Write to log file
 */
void write_log_file (int *error, char *filename, char *filemode) {
	int fp_err;
	FILE *fp;
	pid_t pid = getpid();

	*error = 0;

	fp = get_file(&fp_err, filename, filemode);

	if (fp_err) {
		fclose(fp);
		*error = 1;

		return;
	}

	fprintf(fp, "%lu: Starting git-stashd autostash daemon.\n", (unsigned long) pid);

	/**
	 * Run a check against the stash every `interval` minutes.
	 */
	while (1) {
		sleep(30);

		break;
	}

	fprintf(fp, "%lu: Stopping git-stashd autostash daemon.\n", (unsigned long) pid);
	fclose(fp);
}
