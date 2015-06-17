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
int sleep_secs;
char git_dir[256];
char notify_command[256];
void loop(const char *gitd_directory);
int load_config(const char *path);

int main(int argc, char **argv)
{
	pid_t sid, pid = fork();
	check_less_zero(pid);
	if (pid > 0)
		exit(EXIT_SUCCESS);

	sid = setsid();
	check_less_zero(sid);

	if (argc < 2)
		load_config("/etc/gitd/gitd.conf");
	else
		load_config(argv[1]);
	while (1)
		loop(git_dir);
	exit(EXIT_SUCCESS);
}
void loop(const char *gitd_directory)
{
	char file_buf[256];
	chdir(gitd_directory);
	DIR *cwd = opendir(gitd_directory);
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
		system(notify_command);
		check_less_zero(chdir(gitd_directory));
	}
	closedir(cwd);
	sleep(sleep_secs);
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
int load_config(const char *location)
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
