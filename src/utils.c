/**
 * utils.c
 *
 * Copyright (C) 2017 Nickolas Burr <nickolasburr@gmail.com>
 */

#include "utils.h"

/**
 *
 * String utilities (syntactic sugar, mostly)
 *
 */

/**
 * `strcmp` wrapper
 */
int compare (char *one, char *two) {
	return strcmp(one, two);
};

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
DIR *get_dir (const char *path, int *error) {
	DIR *dp;

	*error = 0;

	if (!(dp = opendir(path))) {
		*error = 1;
	}

	return dp;
}

/**
 * Get pointer to file by its pathname.
 */
FILE *get_file (const char *filename, const char *filemode, int *error) {
	FILE *fp = fopen(filename, filemode);

	*error = 0;

	if (is_null(fp)) {
		*error = 1;
	}

	return fp;
}

/**
 * Get pointer to pipe.
 */
FILE *get_pipe (char *command, char *pipemode, int *error) {
	FILE *fp = popen(command, pipemode);

	*error = 0;

	if (is_null(fp)) {
		*error = 1;
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
	int is_dir, error;
	DIR *dp = get_dir(path, &error);

	if (!error) {
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

/**
 *
 * Type utilities
 *
 */

int is_null (void *ptr) {
	if (ptr == NULL) {
		return 1;
	}

	return 0;
}
