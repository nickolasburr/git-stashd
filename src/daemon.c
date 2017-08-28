/**
 * daemon.c
 *
 * Copyright (C) 2017 Nickolas Burr <nickolasburr@gmail.com>
 */

#include "daemon.h"

/**
 * Fork parent process and run in detached daemon mode.
 */
void fork_proc () {
	int fd;

	switch (fork()) {
		case 0:
			break;
		case -1:
			exit(EXIT_FAILURE);
		default:
			exit(EXIT_SUCCESS);
	}

	if (setsid() < 0) {
		exit(EXIT_FAILURE);
	}

	signal(SIGHUP, SIG_IGN);
	signal(SIGCHLD, SIG_IGN);

	switch (fork()) {
		case 0:
			break;
		case -1:
			exit(EXIT_FAILURE);
		default:
			exit(EXIT_SUCCESS);
	}

	umask(0);

	chdir("/");

	for (fd = sysconf(_SC_OPEN_MAX); fd > 0; fd--) {
		close(fd);
	}

	stdin  = fopen("/dev/null", "r");
	stdout = fopen("/dev/null", "w+");
	stderr = fopen("/dev/null", "w+");
}

/**
 * Create log file.
 */
void touch_log_file (int *error, char *log_file, char *filemode) {
	FILE *fp;
	int fp_err;

	*error = 0;

	/**
	 * Remove trailing slash from path, if present.
	 */
	if (log_file[strlen(log_file) - 1] == '/') {
		log_file[strlen(log_file) - 1] = 0;
	}

	fp = get_file(&fp_err, log_file, filemode);

	if (fp_err) {
		*error = 1;
	}

	fclose(fp);
}

/**
 * Write to log file.
 */
void write_log_file (int *error, char *filename, char *filemode, char *message) {
	int fp_err;
	FILE *fp;
	pid_t pid = getpid();

	*error = 0;

	fp = get_file(&fp_err, filename, filemode);

	if (fp_err) {
		*error = 1;
	} else {
		fprintf(fp, "%zu: %s\n", pid, message);
	}

	fclose(fp);
}
