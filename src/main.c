
#include "stdio.h"
#include "stdlib.h"
#include "sys/types.h"
#include "sys/stat.h"
#include "unistd.h"

pid_t init(void);
void load_conf_file(char ***repositories);

int main(int argc, char **argv)
{
	char **repositories = NULL;
	pid_t pid = init();
	pid_t sid;

	load_conf_file(&repositories);

	if (repositories == NULL)
		exit(EXIT_FAILURE);

	umask(0);

	/* TODO: Load configuration file and clone/pull repositories */

	/* ls /etc/gitd/ */

	/* create session id for child process */
	sid = setsid();
	if (sid < 0)
		exit(EXIT_FAILURE);

	while (1) {
		FILE *f = popen("/bin/git fetch", "r");
		if (f == NULL)
			exit(EXIT_FAILURE);

		/* process the output of git fetch with fgets */

		pclose(f);
		system("/bin/wall");
		sleep(30);
	}
	exit(EXIT_SUCCESS);
}

pid_t init(void)
{
	pid_t pid = fork();
	if (pid < 0)
		exit(EXIT_FAILURE);
	if (pid > 0)
		exit(EXIT_SUCCESS);
	return pid;
}

void load_conf_file(char ***repositories)
{
	FILE *conf = fopen("/etc/gitd.conf", "r");
	if (conf == NULL)
		exit(EXIT_FAILURE);
	fclose(conf);
}
