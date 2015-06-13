#include "dirent.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "sys/types.h"
#include "sys/stat.h"
#include "unistd.h"

char err_buf[100];
#define check_null(A) { \
		if (A == NULL) \
			exit(EXIT_FAILURE); }
#define check_less_zero(A) {   \
		if (A < 0) \
			exit(EXIT_FAILURE); }
void send_message(FILE *f);
void loop(void);

char gitd_directory[256];

int main(int argc, char **argv)
{
	pid_t sid, pid = fork();
	check_less_zero(pid);
	if (pid > 0)
		exit(EXIT_SUCCESS);

	sid = setsid();
	check_less_zero(sid);

	sprintf(gitd_directory, "%s/.gitd/", getenv("HOME"));
	check_less_zero(chdir(gitd_directory));

	while (1)
		loop();

	exit(EXIT_SUCCESS);
}

/*
 * If we don't need to update, just return
 */
void send_message(FILE *f)
{
	char buffer[80];
	char msg_buf[100];
	while (fgets(buffer, sizeof(buffer)-1, f) != NULL) {

		/* search for commit message */
		sprintf(msg_buf, "/usr/bin/wall \"%s\"", buffer);
		check_less_zero(system(msg_buf));
		return;
	}
}

void loop(void)
{
	DIR *cwd = opendir(".");
	struct dirent *entry = NULL;
	struct stat st;
	check_null(cwd);
        for (entry = readdir(cwd); entry != NULL; entry = readdir(cwd)) {
		FILE *f = NULL;

		if (!strcmp(entry->d_name, ".."))
			continue;
		if (!strcmp(entry->d_name, "."))
			continue;
		if (stat(entry->d_name, &st) != 0 || !(S_ISDIR(st.st_mode)))
			continue;
	      	/* printf("dir: %s\n", entry->d_name); */
		check_less_zero(chdir(entry->d_name));
		f = popen("/usr/bin/git fetch", "r");
		sleep(5);

		send_message(f);
		check_less_zero(pclose(f));
		check_less_zero(chdir(gitd_directory));
	}
	closedir(cwd);
	sleep(30);
}
