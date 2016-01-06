#include <stdlib.h>
#include <unistd.h>

#include "gitd.h"

int is_git_directory(const char *path)
{
	return strcmp(path, "..") && strcmp(path, ".") && is_dir(path) == 1;
}

int check_git_repo(void)
{
	int ret = 0;
	char file_buf[256];
	char *check_cmd = getenv("GIT_CHECK");
	FILE *f = popen(check_cmd, "r");
	if (!f) {
		lperror("popen");
		return -1;
	}
	if (fgets(file_buf, sizeof(file_buf), f)) {
		ret = 1;
	}
	if (pclose(f) < 0) {
		lperror("pclose");
		ret = -1;
	}
	return ret;
}

/*
 * This not use system(3) because
 * it blocks SIGCHLD, which is needed
 * to reap zombies. So, this forks
 * and calls execl(3) just like system(3)
 * but doesn't block SIGCHLD.
 */
int notify(void)
{
	pid_t pid = fork();
	if (pid < 0) {
		lperror("could not fork");
		return -1;
	} else if (pid == 0) {
		execl("/bin/sh", "sh", "-c", getenv("NOTIFY"), NULL);
		exit(EXIT_SUCCESS);
	}
	return 0;
}
