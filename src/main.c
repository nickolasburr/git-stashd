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
	/**
	 * If the `--help` option was given, display usage details and exit.
	 */
	if (in_array(GIT_STASHD_USAGE_OPT, argv, argc)) {
		pfusage();

		printf("%d\n", is_dir("/var/log"));

		exit(EXIT_SUCCESS);
	}

	/**
	 * If the `--daemon` option was given, start a daemon.
	 */
	if (in_array("--daemon", argv, argc)) {
		stashd("/var/log");
	}

	FILE *fp = fopen(GIT_STASHD_LOG_FILE, GIT_STASHD_LOG_MODE);

	if (!fp) {
		printf("Error opening file %s\n", GIT_STASHD_LOG_FILE);

		exit(EXIT_FAILURE);
	}

	while (1) {
		fprintf(fp, "git-stashd started.\n");
		sleep(10);

		break;
	}

	fprintf(fp, "git-stashd terminated.\n");
	fclose(fp);

	return EXIT_SUCCESS;
}
