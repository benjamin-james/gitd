#include <signal.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

#include "gitd.h"

int setup_reaper(void)
{
	struct sigaction sa;
	sa.sa_handler = &sigchld_handler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART | SA_NOCLDSTOP;
	if (sigaction(SIGCHLD, &sa, 0) == -1) {
		lperror("sigaction");
		return -1;
	}
	return 0;
}

void sigchld_handler(int signum)
{
	while (waitpid(-1, 0, WNOHANG) > 0);
}

int do_for_repo(const char *dir)
{
	int ret = 0;
	if (!strcasecmp(getenv("FOREACH_REPO"), "thread")) {
		lprintf("Not implemented yet");
	} else if (!strcasecmp(getenv("FOREACH_REPO"), "fork")) {
		pid_t pid = fork();
		if (pid < 0) {
			ret = -1;
		} else if (pid == 0) {
			ret = repo(dir);
			exit(EXIT_SUCCESS);
		}
	} else {
		ret = repo(dir);
	}
	return ret;
}

int do_for_loop(void)
{
	int ret = 0;
	if (!strcasecmp(getenv("FOREACH_LOOP"), "thread")) {
		lprintf("Not implemented yet");
	} else if (!strcasecmp(getenv("FOREACH_LOOP"), "fork")) {
		pid_t pid = fork();
		if (pid < 0) {
			ret = -1;
		} else if (pid == 0) {
			ret = loop();
			exit(EXIT_SUCCESS);
		}
	} else {
		loop();
	}
	return ret;
}

int is_file(const char *path)
{
	struct stat st;
	if (stat(path, &st) < 0) {
		lperror("stat");
		return -1;
	}
	return S_ISREG(st.st_mode);
}

int is_dir(const char *path)
{
	struct stat st;
	if (stat(path, &st) < 0) {
		lperror("stat");
		return -1;
	}
	return S_ISDIR(st.st_mode);
}
