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

	for (
		fd = sysconf(_SC_OPEN_MAX);
		fd > 0;
		fd--
	) {
		close(fd);
	}

	/**
	 * @note extern log_path declared in common.h, defined in main.c.
	 */
	stdin  = fopen(DEV_NULL, "r");
	stdout = fopen(log_path, GIT_STASHD_LOG_MODE);
	stderr = fopen(log_path, GIT_STASHD_LOG_MODE);
}

/**
 * Create regular file.
 */
void ftouch (
	int *error,
	char *filename,
	const char *filemode
) {
	FILE *fp = NULL;
	int fp_err, lchar;

	*error = 0;
	lchar = length(filename) - 1;

	/**
	 * Remove trailing slash from path, if present.
	 * e.g. $HOME/alternate.log/ -> $HOME/alternate.log
	 */
	if (filename[lchar] == '/') {
		filename[lchar] = 0;
	}

	fp = get_file(
		&fp_err,
		filename,
		filemode
	);

	if (fp_err) {
		*error = 1;
	}

	fclose(fp);
}

/**
 * Write to log file.
 */
void flog (const char *msg) {
	fprintf(
		stdout,
		"[%d] %s\n",
		getpid(),
		msg
	);

	/**
	 * To prevent buffer data from lingering in
	 * daemon processes, manually flush stdout.
	 */
	fflush(stdout);
}
