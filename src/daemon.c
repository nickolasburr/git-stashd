/**
 * daemon.c
 *
 * Copyright (C) 2017 Nickolas Burr <nickolasburr@gmail.com>
 */

#include "common.h"
#include "daemon.h"

/**
 * Determine if a pathname points to a valid directory.
 *
 * @notes Adapted from https://goo.gl/ZmWfbx
 */
int is_dir (const char *path) {
	DIR *dp = opendir(path);
	struct dirent *de;
	int is_dir;

	while ((de = readdir(dp))) {
	#ifdef _DIRENT_HAVE_D_TYPE
		if (de->d_type != DT_UNKNOWN && de->d_type != DT_LNK) {
			is_dir = (de->d_type == DT_DIR) ? 1: 0;
		} else
	#endif
		{
			struct stat statbuf;

			stat(de->d_name, &statbuf);

			is_dir = S_ISDIR(statbuf.st_mode);
		}

		if (is_dir) {
			printf("%s/\n", de->d_name);
		}
	}

	return is_dir;
}

pid_t start_daemon (const char *path) {
	int x;
	pid_t pid;

	pid = fork();

	if (pid < 0) {
		exit(EXIT_FAILURE);
	}

	if (pid > 0) {
		exit(EXIT_SUCCESS);
	}

	if (setsid() < 0) {
		exit(EXIT_FAILURE);
	}

	signal(SIGCHLD, SIG_IGN);
	signal(SIGHUP, SIG_IGN);

	pid = fork();

	if (pid < 0) {
		exit(EXIT_FAILURE);
	}

	if (pid > 0) {
		exit(EXIT_SUCCESS);
	}

	umask(0);

	for (x = sysconf(_SC_OPEN_MAX); x >= 0; x--) {
		close(x);
	}

	return pid;
}

void stop_daemon (pid_t pid) {
	kill(pid, SIGKILL);
}

int write_log_entry () {
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
