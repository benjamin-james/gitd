#include "dirent.h"
#include "stdarg.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "syslog.h"
#include "sys/stat.h"
#include "sys/types.h"
#include "unistd.h"

#define check_null(A) { \
		if (A == NULL) \
			exit(EXIT_FAILURE); }
#define check_less_zero(A) {   \
		if (A < 0) \
			exit(EXIT_FAILURE); }
void loop(const char *gitd_directory, const char *notify_command);
int load_config(const char *location, char *notify_command, char *git_dir, int sleep_secs);

int main(int argc, char **argv)
{
	pid_t sid, pid;
	int sleep_secs, forking = 1;
	char notify_command[256], git_dir[256];
	if (argc > 1 && !strcmp(argv[1], "--nofork"))
		forking = 0;
	if (forking) {
		pid = fork();
		check_less_zero(pid);
		if (pid > 0)
			exit(EXIT_SUCCESS);
		sid = setsid();
		check_less_zero(sid);
	}
	openlog(*argv, LOG_PID, LOG_DAEMON);
	load_config(CONFDIR "/gitd.conf", notify_command, git_dir, sleep_secs);
	syslog(LOG_DEBUG, "Loaded config");

	while (1) {
		loop(git_dir);
		sleep(sleep_secs);
	}
	exit(EXIT_SUCCESS);
}

/*
 * self explanatory
 */
int is_git_directory(const char *path)
{
	struct stat st;
	if (!strcmp(path, "..") || !strcmp(path, "."))
		return 0;
	if (stat(path, &st) != 0 || !(S_ISDIR(st.st_mode)))
		return 0;
	return 1;
}

/*
 * Checks if the git repository we are in
 * has updated remotely with "git fetch"
 */
int is_updated(void)
{
	int upd = 0;
	char file_buf[256];
	FILE *f = popen("git fetch 2>&1", "r");
	if (fgets(file_buf, sizeof(file_buf), f))
		upd = 1;
	check_less_zero(pclose(f));
	return upd;
}
/*
 * self explanatory
 */
int notify(const char *notify_command)
{
	int er = 0;
	syslog(LOG_DEBUG, "Notifying user with %s", notify_command);
	er = system(notify_command);
	return er;
}
/*
 * Reads the git directory, checks for updates via 'git fetch', and
 * parses output to determine if an update has occured. If it has,
 * it executes the notify_command.
 */
void loop(const char *gitd_directory, const char *notify_command)
{
	int er = chdir(gitd_directory);
	DIR *cwd = opendir(gitd_directory);
	struct dirent *entry = NULL;
	syslog(LOG_DEBUG, "in loop");
	check_null(cwd);
        for (entry = readdir(cwd); entry != NULL; er = chdir(git_dir), entry = readdir(cwd)) {
		if (is_git_directory(entry->d_name) == 0)
			continue;
		check_less_zero(chdir(entry->d_name));
		if (is_updated())
			check_less_zero(notify(notify_command));
	}
	closedir(cwd);
	syslog(LOG_DEBUG, "Sleeping");
}
void replace_str(char *str, char *original, char *tok)
{
	char token[256], buffer[256];
	sprintf(token, "$(%s)", tok);
	char *p = strstr(str, original);
	if (p == NULL)
		return;
	strncpy(buffer, str, p - str);
	buffer[p - str] = '\0';
	sprintf(buffer + (p - str), "%s%s", token, p + strlen(original));
	strcpy(str, buffer);
}
int load_config(const char *location, char *notify_command, char *git_dir, int sleep_secs)
{
        FILE *f = fopen(location, "r");
	char *key, *token, buffer[256];
	if (f == NULL)
		return -1;
	while (fgets(buffer, sizeof(buffer), f) != NULL) {
		if ((token = strchr(buffer, '#')) != NULL)
			*token = '\0';
		if ((token = strchr(buffer, '=')) == NULL)
			continue;
		*token++ = '\0';
		while (*token == ' ' || *token == '\t')
			token++;
		int tlen = strlen(token);
		if (token[tlen-1] == '\n')
			token[--tlen] = '\0';
		if ((key = strstr(buffer, "Notify")) != NULL)
			memcpy(notify_command, token, tlen + 1);
		else if ((key = strstr(buffer, "Gitdir")) != NULL)
			memcpy(git_dir, token, tlen + 1);
		else if ((key = strstr(buffer, "Sleep")) != NULL)
			sleep_secs = atoi(token);
		else if ((key = strstr(buffer, "Title")) != NULL)
			replace_str(notify_command, "%T", token);
		else if ((key = strstr(buffer, "Body")) != NULL)
			replace_str(notify_command, "%B", token);
	}
	fclose(f);
	return 0;
}
