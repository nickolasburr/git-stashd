/**
 * utils.c
 *
 * Copyright (C) 2017 Nickolas Burr <nickolasburr@gmail.com>
 */

#include "utils.h"

/**
 *
 * String utilities
 *
 */

/**
 * `strcat` wrapper
 */
char *concat (char *buf, char *str) {
	return strcat(buf, str);
}

/**
 * `strcpy` wrapper
 */
char *copy (char *buf, char *str) {
	return strcpy(buf, str);
}

/**
 *
 * Filesystem utilities
 *
 */

/**
 * Get pointer to directory by its pathname.
 */
DIR *get_dir (const char *path) {
	DIR *dp;

	if (!(dp = opendir(path))) {
		printf("Error opening directory %s\n", path);

		exit(EXIT_FAILURE);
	}

	return dp;
}

/**
 * Get pointer to file by its pathname.
 */
FILE *get_file (const char *filename, const char *filemode) {
	FILE *fp = fopen(filename, filemode);

	if (!fp) {
		printf("Error opening file %s\n", filename);

		exit(EXIT_FAILURE);
	}

	return fp;
}

/**
 * Determine if pathname is a directory.
 *
 * @notes Adapted from https://goo.gl/ZmWfbx
 */
int is_dir (const char *path) {
	struct dirent *de;
	int is_dir;
	DIR *dp = get_dir(path);

	while ((de = readdir(dp))) {
	#ifdef _DIRENT_HAVE_D_TYPE
		if (de->d_type != DT_UNKNOWN && de->d_type != DT_LNK) {
			is_dir = (de->d_type == DT_DIR);
		} else
	#endif
		{
			struct stat st;

			stat(de->d_name, &st);
			is_dir = S_ISDIR(st.st_mode);
		}

		if (is_dir) {
			return 1;
		}
	}

	return 0;
}

/**
 * Determine if pathname is a regular file.
 *
 * @notes Adapted from https://goo.gl/ZmWfbx
 */
int is_file (const char *path) {
	struct stat st;

	if (stat(path, &st) == 0 && S_ISREG(st.st_mode)) {
		return 1;
	}

	return 0;
}
