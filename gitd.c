#include "dirent.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "syslog.h"
#include "sys/types.h"
#include "sys/stat.h"
#include "unistd.h"

#define check_null(A) { \
		if (A == NULL) \
			exit(EXIT_FAILURE); }
#define check_less_zero(A) {   \
		if (A < 0) \
			exit(EXIT_FAILURE); }

void send_message(FILE *f, const char *name);
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

	fclose(stdout);
	fclose(stderr);
	fclose(stdin);
	if (chdir(gitd_directory) < 0)
		check_less_zero(mkdir(gitd_directory, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH));

	while (1)
		loop(gitd_directory);

	exit(EXIT_SUCCESS);
}

/*
 * If we don't need to update, just return
 */
void send_message(FILE *f, const char *name)
{
	char file_buf[256];
	if (fgets(file_buf, sizeof(file_buf), f) == NULL)
		return;
	sprintf(file_buf, "echo \'\"%s\" is updated\' | wall", name);
	system(file_buf);
}

void loop(const char *gitd_directory)
{
	DIR *cwd = opendir(".");
	struct dirent *entry = NULL;
	struct stat st;
	check_null(cwd);
        for (entry = readdir(cwd); entry != NULL; entry = readdir(cwd)) {
		FILE *f = NULL;
		if (!strcmp(entry->d_name, "..") || !strcmp(entry->d_name, ".") || stat(entry->d_name, &st) != 0 || !(S_ISDIR(st.st_mode)))
			continue;
		check_less_zero(chdir(entry->d_name));
		f = popen("/usr/bin/git fetch 2>&1", "r");
		send_message(f, entry->d_name);
		check_less_zero(pclose(f));
		check_less_zero(chdir(gitd_directory));
	}
	closedir(cwd);
	sleep(30);
}
