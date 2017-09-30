/**
 * signal.c
 *
 * Copyright (C) 2017 Nickolas Burr <nickolasburr@gmail.com>
 */

#include "signals.h"

/**
 * Run cleanup on SIGHUP or SIGINT.
 */
void on_signal (int signal) {
	switch (signal) {
		case SIGHUP:
			fprintf(stdout, "Caught SIGHUP, hanging up...\n");

			/**
			 * @note: Extern lock_file declared in common.h, defined in main.c.
			 * @todo: Move this to dedicated function.
			 */
			if (is_file(lock_file)) {
				unlink(lock_file);
			}

			break;
		case SIGINT:
			fprintf(stdout, "Caught SIGINT, terminating...\n");

			/**
			 * @note: Extern lock_file declared in common.h, defined in main.c.
			 * @todo: Move this to dedicated function.
			 */
			if (is_file(lock_file)) {
				unlink(lock_file);
			}

			exit(EXIT_SUCCESS);
		default:
			fprintf(stderr, "Caught wrong signal: %d\n", signal);
			return;
	}
}

/**
 * Run tasks during awake period.
 */
void on_awake (int signal) {
	if (signal != SIGALRM) {
		fprintf(stderr, "Caught wrong signal: %d\n", signal);
	}

	/**
	 * @todo: Finish building this.
	 */
}

/**
 * Augmented sleep function with signal handling.
 */
void nap (int seconds) {
	struct sigaction action;
	sigset_t mask;

	action.sa_handler = &on_awake;
	action.sa_flags = SA_RESETHAND;

	sigfillset(&action.sa_mask);
	sigaction(SIGALRM, &action, NULL);
	sigprocmask(0, NULL, &mask);
	sigdelset(&mask, SIGALRM);

	alarm(seconds);
	sigsuspend(&mask);
}
