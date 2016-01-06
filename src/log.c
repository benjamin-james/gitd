#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include "gitd.h"

static FILE *_fp;

/*
 * Yes, before you ask, I know what I/O redirection is.
 * This just seems less hacky than adding 'gitd >log'
 * in the crontab, etc.
 */
int open_log(void)
{
	int ret = 0;
	const char *logfile = getenv("LOG_FILE");
	if (!strcmp(logfile, "syslog")) {
		openlog(getenv("PROGRAM_NAME"), LOG_PID, LOG_USER);
	} else {
		_fp = fopen(logfile, "w");
		ret = (_fp == NULL) ? -1 : 0;
	}
	return ret;
}
int lperror(const char *str)
{
#define ERR_STRLEN 512
	char err_str[ERR_STRLEN + 1];
	strerror_r(errno, err_str, ERR_STRLEN);
	lprintf("%s: %s", str, err_str);
#undef ERR_STRLEN
	return 0;
}

int lputs(const char *str)
{
	int ret;
	va_list args;
	const char *quiet = getenv("QUIET");
	if (quiet && !strcasecmp(quiet, "true")) {
		return 0;
	}
	const char *logfile = getenv("LOG_FILE");
	if (!strcmp(logfile, "syslog")) {
		syslog(LOG_INFO, "%s", str);
	} else if (_fp) {
		fprintf(_fp, "%s\n", str);
	}
	return ret;
}

int close_log(void)
{
	if (_fp) {
		return fclose(_fp);
	}
	return 0;
}
