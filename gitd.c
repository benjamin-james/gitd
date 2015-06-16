#include "dirent.h"
#include "stdarg.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "sys/stat.h"
#include "sys/types.h"
#include "unistd.h"

#define check_null(A) { \
		if (A == NULL) \
			exit(EXIT_FAILURE); }
#define check_less_zero(A) {   \
		if (A < 0) \
			exit(EXIT_FAILURE); }
#define SLEEP_TIME 60
/*
 * SEND for now uses notify-send,
 * which takes two arguments, the
 * title and the message.
 */
#define SEND "notify-send \"$(basename $(dirname $PWD))\" \"$(git log -n 1 --pretty=format:'%d%n%an%n%s')\""

void loop(const char *gitd_directory);

int main(int argc, char **argv)
{
	pid_t sid, pid = fork();
	char gitd_directory[256];
	check_less_zero(pid);
	if (pid > 0)
		exit(EXIT_SUCCESS);

	sid = setsid();
	check_less_zero(sid);
	sprintf(gitd_directory, "%s/.gitd/", getenv("HOME"));

	if (chdir(gitd_directory) < 0)
		check_less_zero(mkdir(gitd_directory, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH));

	while (1)
		loop(gitd_directory);

	exit(EXIT_SUCCESS);
}

void loop(const char *gitd_directory)
{
	char file_buf[256];
	DIR *cwd = opendir(".");
	struct dirent *entry = NULL;
	struct stat st;
	check_null(cwd);
        for (entry = readdir(cwd); entry != NULL; entry = readdir(cwd)) {
		FILE *f = NULL;
		if (!strcmp(entry->d_name, "..") || !strcmp(entry->d_name, ".") || stat(entry->d_name, &st) != 0 || !(S_ISDIR(st.st_mode)))
			continue;
		check_less_zero(chdir(entry->d_name));
		f = popen("git fetch 2>&1", "r");
		if (fgets(file_buf, sizeof(file_buf), f) == NULL)
			continue;
		check_less_zero(pclose(f));
		check_less_zero(system(SEND));
		check_less_zero(chdir(gitd_directory));
	}
	closedir(cwd);
	sleep(SLEEP_TIME);
}
