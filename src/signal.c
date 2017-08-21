/**
 * signal.c
 *
 * Copyright (C) 2017 Nickolas Burr <nickolasburr@gmail.com>
 */

#include "signal.h"

void on_signal (int signal) {
	const char *signal_name;
	sigset_t pending;

	if (signal == SIGHUP) {
		signal_name = "SIGHUP";
	}

	if (signal == SIGUSR1) {
		signal_name = "SIGUSR1";
	}

	/**
	 * Exit cleanly on SIGINT
	 */
	if (signal == SIGINT) {
		printf("Caught SIGINT, exiting...\n");

		exit(EXIT_SUCCESS);
	}

	if (!signal_name) {
		fprintf(stderr, "Caught wrong signal: %d\n", signal);

		return;
	}

	printf("Caught %s, napping for ~5 seconds\n", signal_name);
	nap(5);

	/**
	 * Get pending signals
	 */
	sigpending(&pending);

	if (sigismember(&pending, SIGHUP)) {
		printf("A SIGHUP is pending...\n");
	}

	if (sigismember(&pending, SIGUSR1)) {
		printf("A SIGUSR1 is pending...\n");
	}

	printf("Done handling %s\n\n", signal_name);
}

void on_sigalrm (int signal) {
	if (signal != SIGALRM) {
		fprintf(stderr, "Caught wrong signal: %d\n", signal);
	}

	printf("Caught SIGALRM, nap() will end...\n");
}

void nap (int seconds) {
	struct sigaction action;
	sigset_t mask;

	action.sa_handler = &on_sigalrm;
	action.sa_flags = SA_RESETHAND;

	sigfillset(&action.sa_mask);
	sigaction(SIGALRM, &action, NULL);

	/**
	 * Get signal mask, unblock SIGALRM
	 */
	sigprocmask(0, NULL, &mask);
	sigdelset(&mask, SIGALRM);

	alarm(seconds);
	sigsuspend(&mask);

	if (alarm_fired) {
		printf("sigsuspend() returned\n");
	}
}
