/**
 * daemon.c
 *
 * Copyright (C) 2017 Nickolas Burr <nickolasburr@gmail.com>
 */

#include "argv.h"
#include "common.h"
#include "daemon.h"

/**
 * Determine if a pathname points to a valid directory.
 *
 * @notes Adapted from https://goo.gl/ZmWfbx
 */
int is_dir (const char *path) {
	DIR *dp;
	struct dirent *de;
	int is_dir;

	char actualpath[PATH_MAX], *ptr;

	if (!(dp = opendir(path))) {
		return 0;
	}

	ptr = realpath(path, actualpath);
	printf("is_dir -> path -> %s\n", ptr);

	while ((de = readdir(dp))) {
	#ifdef _DIRENT_HAVE_D_TYPE
		if (de->d_type != DT_UNKNOWN && de->d_type != DT_LNK) {
			is_dir = (de->d_type == DT_DIR) ? 1 : 0;
		} else
	#endif
		{
			struct stat statbuf;

			stat(de->d_name, &statbuf);
			is_dir = S_ISDIR(statbuf.st_mode);
		}

		if (is_dir) {
			printf("%s/\n", de->d_name);

			return 1;
		}
	}

	return 0;
}

/**
 * Start daemon process
 */
void start_daemon (const char *repo, long *pid) {
	int x;
	pid_t fpid;

	fpid = fork();

	if (fpid < 0) {
		exit(EXIT_FAILURE);
	}

	if (fpid > 0) {
		exit(EXIT_SUCCESS);
	}

	if (setsid() < 0) {
		exit(EXIT_FAILURE);
	}

	signal(SIGCHLD, SIG_IGN);
	signal(SIGHUP, SIG_IGN);

	fpid = fork();

	if (fpid < 0) {
		exit(EXIT_FAILURE);
	}

	if (fpid > 0) {
		exit(EXIT_SUCCESS);
	}

	umask(0);

	for (x = sysconf(_SC_OPEN_MAX); x >= 0; x--) {
		close(x);
	}

	chdir(repo);

	*pid = (long) fpid;
}

/**
 * Kill daemon process
 */
void stop_daemon (long *pid) {
	kill((long) *pid, SIGKILL);
}

/**
 * Get pointer to log file
 */
FILE *get_log_file (char *filename, char *filemode) {
	FILE *fp = fopen(filename, filemode);

	if (!fp) {
		printf("Error opening file %s\n", filename);

		exit(EXIT_FAILURE);
	}

	return fp;
}

/**
 * Write to log file
 */
int write_log_file (char *filename, char *filemode) {
	FILE *fp = get_log_file(filename, filemode);

	/**
	 * We'll skip error checking explicitly,
	 * as it's addressed in `get_log_file` and
	 * will exit if an error is encountered.
	 */
	while (1) {
		fprintf(fp, "git-stashd started.\n");
		sleep(10);

		break;
	}

	fprintf(fp, "git-stashd terminated.\n");
	fclose(fp);

	return EXIT_SUCCESS;
}
