#include "dirent.h"
#include "stdio.h"
#include "stdlib.h"
#include "sys/types.h"
#include "sys/stat.h"
#include "unistd.h"

#define check_null(A) { \
		if (A == NULL) \
			exit(EXIT_FAILURE); }
#define check_less_zero(A) {   \
		if (A < 0) \
			exit(EXIT_FAILURE); }
void send_message(FILE *f);

int main(int argc, char **argv)
{
	pid_t sid, pid = fork();
	check_less_zero(pid);
	if (pid > 0)
		exit(EXIT_SUCCESS);

	umask(0);

	sid = setsid();
	check_less_zero(sid);
	check_less_zero(chdir("~/.gitd"));

	while (1) {
		DIR *cwd = opendir(".");
		struct dirent *entry = NULL;
		struct stat st;
		check_null(cwd);
		entry = readdir(cwd);
		check_null(cwd);
		while (entry != NULL) {
			if (stat(entry->d_name, &st) == 0 && S_ISDIR(st.st_mode)) {
				FILE *f = NULL;
				check_less_zero(chdir(entry->d_name));
				f = popen("/usr/bin/git fetch", "r");
				check_null(f);
				send_message(f);
				check_less_zero(pclose(f));
				check_less_zero(chdir("../"));
			}
			entry = readdir(cwd);
		}
		closedir(cwd);

		sleep(30);
	}

	exit(EXIT_SUCCESS);
}

/*
 * If we don't need to update, just return
 */
void send_message(FILE *f)
{

}
