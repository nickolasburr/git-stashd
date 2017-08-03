/**
 * main.c
 *
 * Copyright (C) 2017 Nickolas Burr <nickolasburr@gmail.com>
 */

#include "common.h"

int main () {

	/**
	 * Start daemon process
	 */
	stashd();

	FILE *fp = fopen(GIT_STASHD_LOGFILE, "a+");

	if (fp == NULL) {
		printf("Error opening file %s\n", GIT_STASHD_LOGFILE);
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
