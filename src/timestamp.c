/**
 * timestamp.c
 *
 * Copyright (C) 2017 Nickolas Burr <nickolasburr@gmail.com>
 */

#include "timestamp.h"

/**
 * Get timestamp
 */
char *get_timestamp (char *ts_buf) {
	time_t lt = time(NULL);
	struct tm tm = *localtime(&lt);
	static const char *tm_format = "%d:%.2d:%.2d on %.2d-%.2d-%d";
	char *tm_buf;

	tm_buf = ALLOC((sizeof(char) * (strlen(tm_format) + NULL_BYTE)) + GIT_STASHD_TMS_LENGTH_MAX);
	sprintf(tm_buf, tm_format, tm.tm_hour, tm.tm_min, tm.tm_sec, (tm.tm_mon + 1), tm.tm_mday, (tm.tm_year + 1900));

	copy(ts_buf, tm_buf);

	FREE(tm_buf);

	return ts_buf;
}
