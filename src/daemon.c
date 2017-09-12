/**
 * daemon.c
 *
 * Copyright (C) 2017 Nickolas Burr <nickolasburr@gmail.com>
 */

#include "daemon.h"

/**
 * Fork parent process and run in detached daemon mode.
 */
void daemonize (void) {
	int fd;
	uid_t euid;

	euid = geteuid();

	switch (fork()) {
		case 0:
			break;
		case -1:
			exit(EXIT_FAILURE);
		default:
			exit(EXIT_SUCCESS);
	}

	seteuid(euid);

	if (setsid() < 0) {
		exit(EXIT_FAILURE);
	}

	signal(SIGHUP, SIG_IGN);

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

	/**
	 * @notes extern log_path declared in common.h, defined in main.c.
	 */
	stdin  = fopen("/dev/null", "r");
	stdout = fopen(log_path, GIT_STASHD_LOG_MODE);
	stderr = fopen(log_path, GIT_STASHD_LOG_MODE);
}

/**
 * Create log file.
 */
void touch_log_file (int *error, char *filename, const char *filemode) {
	FILE *fp;
	int fp_err;

	*error = 0;

	/**
	 * Remove trailing slash from path, if present.
	 * e.g. /var/log/alternate.log/
	 */
	if (filename[strlen(filename) - 1] == '/') {
		filename[strlen(filename) - 1] = 0;
	}

	fp = get_file(&fp_err, filename, filemode);

	if (fp_err) {
		*error = 1;
	}

	fclose(fp);
}

/**
 * Write to log file.
 */
void write_to_log (int *error, const char *filename, const char *filemode, const char *message) {
	int fp_err;
	FILE *fp;
	pid_t pid = getpid();

	*error = 0;

	fp = get_file(&fp_err, filename, filemode);

	if (fp_err) {
		*error = 1;
	} else {
		fprintf(fp, "[%zu] %s\n", pid, message);
	}

	fclose(fp);
}
