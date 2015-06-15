#include "dirent.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "syslog.h"
#include "sys/types.h"
#include "sys/stat.h"
#include "unistd.h"

#define check_null(A) { \
		if (A == NULL) { \
			exit(EXIT_FAILURE); }}
#define check_less_zero(A) {   \
		if (A < 0) \
			exit(EXIT_FAILURE); }

void send_message(FILE *f, const char *name);
void loop(void);

char gitd_directory[256];

#define _DEBUG_ 1

int main(int argc, char **argv)
{
#ifndef _DEBUG_
	pid_t sid, pid = fork();
	check_less_zero(pid);
	if (pid > 0)
		exit(EXIT_SUCCESS);

	sid = setsid();
	check_less_zero(sid);
#endif
	sprintf(gitd_directory, "%s/.gitd/", getenv("HOME"));
	dup2(STDOUT_FILENO, STDERR_FILENO);
	if (chdir(gitd_directory) < 0)
		check_less_zero(mkdir(gitd_directory, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH));

	while (1)
		loop();

	exit(EXIT_SUCCESS);
}

/*
 * If we don't need to update, just return
 */
void send_message(FILE *f, const char *name)
{
	char file_buf[256];
	int updated = 0;
	while (fgets(file_buf, sizeof(file_buf), f) != NULL)
		updated = 1;
	sprintf(file_buf, "wall \'%s is updated!\'", name);
	system(file_buf);
}

void loop(void)
{
	DIR *cwd = opendir(".");
	struct dirent *entry = NULL;
	struct stat st;
	check_null(cwd);
        for (entry = readdir(cwd); entry != NULL; entry = readdir(cwd)) {
		FILE *f = NULL;

		if (!strcmp(entry->d_name, "..") || !strcmp(entry->d_name, ".") || stat(entry->d_name, &st) != 0 || !(S_ISDIR(st.st_mode)))
			continue;
	      	/* printf("dir: %s\n", entry->d_name); */
		check_less_zero(chdir(entry->d_name));
		f = popen("/usr/bin/git fetch 2>&1", "r");
		send_message(f, entry->d_name);
		check_less_zero(pclose(f));
		check_less_zero(chdir(gitd_directory));
	}
	closedir(cwd);
	sleep(30);
}
