#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "gitd.h"

int set_config_file(void)
{
	char path[PATH_MAX];
	char *str = NULL;
	if ((str = getenv("XDG_CONFIG_HOME"))) {
		snprintf(path, PATH_MAX, "%s/gitd/gitd.conf", str);
	} else if ((str = getenv("HOME"))) {
		snprintf(path, PATH_MAX, "%s/.config/gitd/gitd.conf", str);
	} else {
		return -1;
	}
	return setenv("CONFIG_FILE", path, 0);
}
int set_dir(void)
{
	char path[PATH_MAX];
	char *str = NULL;
	if ((str = getenv("XDG_DATA_HOME"))) {
		snprintf(path, PATH_MAX, "%s/gitd/", str);
	} else if ((str = getenv("HOME"))) {
		snprintf(path, PATH_MAX, "%s/.local/share/gitd/", str);
	} else {
		return -1;
	}
	return setenv("DIR", path, 0);
}

int try_set_env_value(const char *env, const char *value)
{
	if (setenv(env, value, 1) < 0) {
		printf("Error setting %s to %s\n", env, value);
		return -1;
	}
	return 0;
}

int set_default_envvars(const char *argv_0)
{
	if (setenv("PROGRAM_NAME", argv_0, 1) < 0) {
		return -1;
	} if (set_config_file() < 0) {
		return -1;
	} else if (set_dir() < 0) {
		return -1;
	} else if (setenv("SLEEP", "60", 0) < 0) {
		return -1;
	} else if (setenv("NOTIFY", "/usr/bin/notify-send \"`basename $PWD`\" \"`git log -n 1 --pretty=format:'%d%n%an%n%s'`\"", 0) < 0) {
		return -1;
	} else if (setenv("GIT_CHECK", "git fetch", 0) < 0) {
		return -1;
	} else if (setenv("FUNCTION", "loop", 0) < 0) {
		return -1;
	} else if (setenv("FORKING", "true", 0) < 0) {
		return -1;
	} else if (setenv("FOREACH_REPO", "thread", 0) < 0) {
		return -1;
	} else if (setenv("FOREACH_LOOP", "fork", 0) < 0) {
		return -1;
	} else if (setenv("QUIET", "false", 0) < 0) {
		return -1;
	} else if (setenv("LOG_FILE", "/dev/stderr", 0) < 0) {
		return -1;
	}
	return 0;
}

