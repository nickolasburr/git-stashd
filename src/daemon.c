/**
 * daemon.c
 *
 * Copyright (C) 2017 Nickolas Burr <nickolasburr@gmail.com>
 */

#include "daemon.h"

/**
 * Fork parent process and run in detached daemon mode.
 */
void fork_proc (void) {
	int x;

	pid_t pid = fork();

	if (pid < 0) {
		exit(EXIT_FAILURE);
	}

	if (pid > 0) {
		exit(EXIT_SUCCESS);
	}

	if (setsid() < 0) {
		exit(EXIT_FAILURE);
	}

	signal(SIGCHLD, SIG_IGN);
	signal(SIGHUP, SIG_IGN);

	pid = fork();

	if (pid < 0) {
		exit(EXIT_FAILURE);
	}

	if (pid > 0) {
		exit(EXIT_SUCCESS);
	}

	umask(0);

	for (x = sysconf(_SC_OPEN_MAX); x >= 0; x--) {
		close(x);
	}
}

/**
 * Write to log file
 */
void write_log_file (char *filename, char *filemode) {
	int error;
	FILE *fp = get_file(filename, filemode, &error);
	pid_t pid = getpid();

	if (error) {
		exit(EXIT_FAILURE);
	}

	fprintf(fp, "Starting git-stashd daemon with PID %lu.\n", (unsigned long) pid);

	/**
	 * Run a check against the stash every `interval` minutes.
	 */
	while (1) {
		sleep(30);

		break;
	}

	fprintf(fp, "Stopping git-stashd daemon with PID %lu.\n", (unsigned long) pid);
	fclose(fp);
}
