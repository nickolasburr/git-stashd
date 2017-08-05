/**
 * main.c
 *
 * Copyright (C) 2017 Nickolas Burr <nickolasburr@gmail.com>
 */

#include "common.h"
#include "daemon.h"
#include "usage.h"

int main (int argc, char *argv[]) {
	int i;

	pfusage();

	/**
	 * Start daemon process
	 */
	// stashd();

	for (i = 0; i < argc; i += 1) {
		const char *flag = argv[i];
		printf("%s\n", flag);
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
