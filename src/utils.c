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
 * GNU `basename` polyfill.
 */
char *base_name(char *path) {
	char *base = strrchr(path, '/');
	return base ? (base + 1) : path;
}

/**
 * `strcmp` wrapper
 */
int compare(char *one, char *two) {
	return strcmp(one, two);
};

/**
 * `strcat` wrapper
 */
char *concat(char *buf, char *str) {
	return strcat(buf, str);
}

/**
 * `strcpy` wrapper
 */
char *copy(char *buf, char *str) {
	return strcpy(buf, str);
}

/**
 * GNU `dirname` polyfill.
 */
char *dir_name(char *path) {
	static const char *dot = ".";
	char *pslash = NULL;

	/**
	 * Get trailing slash.
	 */
	pslash = !IS_NULL(path)
	       ? strrchr(path, '/') : NULL;

	if (pslash == path) {
		++pslash;
	} else if (!IS_NULL(pslash) && pslash[1] == '\0') {
		pslash = memchr(path, pslash - path, '/');
	}

	if (!IS_NULL(pslash)) {
		pslash[0] = '\0';
	} else {
		path = (char *) dot;
	}

	return path;
}

/**
 * Get index of element in array.
 */
int index_of(
	char *element,
	char **array,
	size_t size
) {
	unsigned int i;

	for (i = 0; i < size; i += 1) {
		if (!compare(array[i], element)) {
			return i;
		}
	}

	return -1;
}

/**
 * Check if element exists in array.
 */
int in_array(
	char *element,
	char **array,
	size_t size
) {
	unsigned int i;

	for (i = 0; i < size; i += 1) {
		if (!compare(array[i], element)) {
			return 1;
		}
	}

	return 0;
}

/**
 * `strlen` wrapper
 */
int length(char *str) {
	return strlen(str);
}

/**
 *
 * Filesystem utilities
 *
 */

/**
 * Get pointer to directory by its pathname.
 */
DIR *get_dir(
	int *error,
	const char *path
) {
	DIR *dir = NULL;

	*error = 0;
	dir = opendir(path);

	if (IS_NULL(dir)) {
		*error = 1;
	}

	return dir;
}

/**
 * Get pointer to file by its pathname.
 */
FILE *get_file(
	int *error,
	const char *name,
	const char *mode
) {
	FILE *file = NULL;

	*error = 0;
	file = fopen(name, mode);

	if (IS_NULL(file)) {
		*error = 1;
	}

	return file;
}

/**
 * Get pointer to pipe.
 */
FILE *open_pipe(
	int *error,
	const char *cmd,
	const char *mode
) {
	FILE *pipe = NULL;

	*error = 0;
	pipe = popen(cmd, mode);

	if (IS_NULL(pipe)) {
		*error = 1;
	}

	return pipe;
}

/**
 * Close pointer to pipe.
 */
int close_pipe(FILE *fp) {
	return pclose(fp);
}

/**
 * Determine if pathname is a directory.
 *
 * @note Adapted from https://goo.gl/ZmWfbx
 */
int is_dir(const char *path) {
	struct dirent *de;
	int is_dir, error;
	DIR *dp = get_dir(&error, path);

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
 * @note Adapted from https://goo.gl/ZmWfbx
 */
int is_file(const char *path) {
	struct stat st;
	return (stat(path, &st) == 0 && S_ISREG(st.st_mode)) ? 1 : 0;
}

/**
 * Determine if pathname is writable.
 */
int is_writable(const char *path) {
	return !access(path, W_OK) ? 1 : 0;
}

/**
 *
 * Type utilities
 *
 */

/**
 * Determine if string is numeric.
 */
int is_numeric(char *str) {
	int index = 0;
	char chrval;

	do {
		chrval = str[index++];

		if (chrval == '\0') {
			break;
		}

		if (!isdigit(chrval)) {
			return 0;
		}
	} while (1);

	return 1;
}
