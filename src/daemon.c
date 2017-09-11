/**
 * daemon.c
 *
 * Copyright (C) 2017 Nickolas Burr <nickolasburr@gmail.com>
 */

#include "daemon.h"

/**
 * Fork parent process and run in detached daemon mode.
 */
void daemonize () {
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

	stdin  = fopen("/dev/null", "r");
	stdout = fopen("/dev/null", "w+");
	stderr = fopen("/dev/null", "w+");
}

/**
 * Send output to syslog for logging.
 */
void write_to_log (const char *name, const char *message, int level) {
	/**
	 * Limit logging to certain level.
	 */
	setlogmask(LOG_UPTO(level));

	openlog(name, LOG_CONS | LOG_PID | LOG_NDELAY, LOG_LOCAL1);
	syslog(level, message);
	closelog();
}
