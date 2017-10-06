/**
 * argv.c
 *
 * Copyright (C) 2017 Nickolas Burr <nickolasburr@gmail.com>
 */

#include "argv.h"

static char *options[GIT_STASHD_NUM_OPT_COUNT] = {
	GIT_STASHD_OPT_HELP_L,
	GIT_STASHD_OPT_HELP_S,
	GIT_STASHD_OPT_FOREGROUND_L,
	GIT_STASHD_OPT_FOREGROUND_S,
	GIT_STASHD_OPT_INTERVAL_L,
	GIT_STASHD_OPT_INTERVAL_S,
	GIT_STASHD_OPT_LOG_FILE_L,
	GIT_STASHD_OPT_LOG_FILE_S,
	GIT_STASHD_OPT_MAX_ENTRIES_L,
	GIT_STASHD_OPT_MAX_ENTRIES_S,
	GIT_STASHD_OPT_PATH_L,
	GIT_STASHD_OPT_PATH_S,
	GIT_STASHD_OPT_VERSION_L,
	GIT_STASHD_OPT_VERSION_S,
};

/**
 * Validate options given via argv.
 */
int has_bad_opts (char **argv, int argc) {
	int index = 1;

	while (index < argc) {
		char *opt = argv[index];

		if (!in_array(opt, options, GIT_STASHD_NUM_OPT_COUNT)) {
			return 1;
		}

		index += 2;
	}

	return 0;
}

/**
 * Print formatted usage message to stdout.
 */
void usage (void) {
	printf("Usage: git-stashd [OPTIONS]\n\nType 'man git-stashd' for details\n");

	exit(EXIT_SUCCESS);
}
