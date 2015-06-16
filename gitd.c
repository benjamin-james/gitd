#include "dirent.h"
#include "stdarg.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "sys/stat.h"
#include "sys/types.h"
#include "unistd.h"

#include "SDL2/SDL.h"

#define check_null(A) { \
		if (A == NULL) \
			exit(EXIT_FAILURE); }
#define check_less_zero(A) {   \
		if (A < 0) \
			exit(EXIT_FAILURE); }
#define SLEEP_TIME 60

void loop(const char *gitd_directory);
void get_output(const char *command, char *ptr);

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
	char title[64];
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
		get_output("basename $(dirname $PWD)", title);
		get_output("git log -n 1 --pretty=format:'%d%n%an%n%s'", file_buf);
		check_less_zero(SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, title, file_buf, NULL));
		check_less_zero(chdir(gitd_directory));
	}
	closedir(cwd);
	sleep(SLEEP_TIME);
}

void get_output(const char *command, char *ptr)
{
	FILE *f = popen(command, "r");
	while (fgets(ptr, sizeof(ptr), f) != NULL);
	pclose(f);
}
