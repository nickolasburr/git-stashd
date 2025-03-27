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
	/**
	 * @note: If we decide to use SIGHUP for an alternate purpose (e.g. reload config)
	 *        we can use queue for handling pending signals that need to be processed.
	 */
	sigset_t queue;
	pid_t pid = getpid();

	switch (signal) {
		case SIGHUP:
			fprintf(stdout, "[%zu] Caught SIGHUP. Running cleanup tasks before exiting.\n", pid);

			/**
			 * @note: Extern lock_file declared in common.h, defined in main.c.
			 */
			if (is_file(lock_file)) {
				fprintf(stdout, "[%zu] --> Removing lock file %s\n", pid, lock_file);

				if (is_error(unlink(lock_file))) {
					fprintf(stdout, "[%zu] --> Unable to remove lock file %s\n", pid, lock_file);
				}
			} else {
				fprintf(stdout, "[%zu] --> Unable to locate lock file %s\n", pid, lock_file);
			}

			fprintf(stdout, "[%zu] Cleanup complete. Exiting...\n", pid);

			/**
			 * Manually flush stdout.
			 */
			fflush(stdout);

			exit(EXIT_SUCCESS);
		case SIGINT:
			fprintf(stdout, "[%zu] Caught SIGINT. Running cleanup tasks before exiting.\n", pid);

			/**
			 * @note: Extern lock_file declared in common.h, defined in main.c.
			 */
			if (is_file(lock_file)) {
				fprintf(stdout, "[%zu] --> Removing lock file %s\n", pid, lock_file);

				if (is_error(unlink(lock_file))) {
					fprintf(stdout, "[%zu] --> Unable to remove lock file %s\n", pid, lock_file);
				}
			} else {
				fprintf(stdout, "[%zu] --> Unable to locate lock file %s\n", pid, lock_file);
			}

			fprintf(stdout, "[%zu] Cleanup complete. Exiting...\n", pid);
			fflush(stdout);
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
