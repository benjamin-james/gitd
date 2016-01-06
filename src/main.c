#include <dirent.h>
#include <errno.h>
#include <limits.h>
#include <stdlib.h>
#include <unistd.h>
#include "gitd.h"

int repo(const char *dir)
{
	int ret = 0;
	char *git_dir = getenv("DIR");
	if (chdir(dir) < 0) {
		lprintf("Could not chdir to %s: %s", dir, strerror(errno));
		return -1;
	}
	ret = check_git_repo();
	if (ret == 1) {
		notify();
	}
	if (chdir(git_dir) < 0) {
	       	lprintf("Could not chdir to %s: %s", git_dir, strerror(errno));
	       	ret = -1;
	}
	return ret;
}

int loop(void)
{
	int ret = 0;
	DIR *cwd = opendir(getenv("DIR"));
	struct dirent *entry = NULL;
	if (!cwd) {
		lprintf("Could not open %s: %s", getenv("DIR"), strerror(errno));
		ret = -1;
	}
	if (!ret && chdir(getenv("DIR")) < 0) {
		lprintf("Could not chdir to %s: %s", getenv("DIR"), strerror(errno));
		ret = -1;
	}
	while (!ret && (entry = readdir(cwd)) != NULL) {
		if (is_git_directory(entry->d_name)) {
			if (do_for_repo(entry->d_name) < 0) {
				ret = -1;
				break;
			}
		}
	}
	return ret;
}

int main(int argc, char **argv)
{
	if (init(argc, argv) < 0) {
		return 1;
	}
	if (!strcasecmp(getenv("FORKING"), "TRUE")) {
		pid_t pid = fork();
		if (pid < 0) {
			lperror("fork");
			exit(EXIT_FAILURE);
		} else if (pid > 0) { /* parent */
			exit(EXIT_SUCCESS);
		}
	}
	if (!strcmp(getenv("FUNCTION"), "loop")) {
		int running = 1;
		unsigned int seconds = 0;
		char *endptr;
		long l = strtol(getenv("SLEEP"), &endptr, 10);
		if (l > UINT_MAX || l <= 0 || *endptr) {
			lprintf("Invalid sleep time %ld\n", l);
		} else {
			seconds = (unsigned int)l;
		}
		while (do_for_loop() == 0) {
			if (seconds) {
				sleep(seconds);
			}
		}
	} else {
		loop();
	}
	close_log();
	_exit(EXIT_SUCCESS);
}

