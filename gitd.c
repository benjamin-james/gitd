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

void send_message(FILE *f);
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
	if (chdir(gitd_directory) < 0)
		check_less_zero(mkdir(gitd_directory, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH));

	while (1)
		loop();

	exit(EXIT_SUCCESS);
}

/*
 * If we don't need to update, just return
 */
void send_message(FILE *f)
{
	char msg_buf[256];
	char file_buf[256];
	fread(file_buf, 1, sizeof(file_buf), f);
	sprintf(msg_buf, "echo \"%s\" > %s/log.txt", file_buf, gitd_directory);
/*	char *buffer = malloc(sz);
	check_null(buffer);
	fread(buffer, 1, sz, f);
	sprintf(msg_buf, "wall \"%s\"", buffer);
	system(msg_buf);
	free(buffer);*/

	system(msg_buf);
	//system("echo \"haha\" > /home/ben/log.txt");
}

void loop(void)
{
	DIR *cwd = opendir(".");
	struct dirent *entry = NULL;
	struct stat st;
	check_null(cwd);
        for (entry = readdir(cwd); entry != NULL; entry = readdir(cwd)) {
		/* FILE *f = NULL; */

		if (!strcmp(entry->d_name, "..") || !strcmp(entry->d_name, ".") || stat(entry->d_name, &st) != 0 || !(S_ISDIR(st.st_mode)))
			continue;
	      	/* printf("dir: %s\n", entry->d_name); */
		check_less_zero(chdir(entry->d_name));
		char buf[256];
		sprintf(buf, "/usr/bin/git fetch > %s/log.txt", getenv("HOME"));
		system(buf);
		/*f = popen("/usr/bin/git fetch 2>&1", "r");

		send_message(f);
		check_less_zero(pclose(f));*/
		check_less_zero(chdir(gitd_directory));
	}
	closedir(cwd);
	sleep(30);
}
