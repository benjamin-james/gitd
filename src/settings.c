#include "stdio.h"
#include "string.h"
#include "stdlib.h"

#include "settings.h"

static char notify_command[256];
static char git_dir[256];
static int sleep_secs;

const char *get_notify_command(void);
const char *get_git_dir(void);
int get_sleep_secs(void);

/*
 * Loads the config file and parses it for
 * Notification and git dir
 *
 * This does dynamically allocate, so please
 * free the resources once finished.
 */
int load_config(const char *file_path)
{
	FILE *f = fopen(file_path, "r");
	char buffer[256];
	char *key, *token;
	if (f == NULL)
		return -1;
	while (fgets(buffer, sizeof(buffer), f) != NULL) {
		token = strchr(buffer, '#');
		if (token)
			*token = '\0';
		token = strchr(buffer, '=');
		if (token == NULL)
			continue;
		*token = '\0';
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
	}
	fclose(f);
	return 0;
}
const char *get_notify_command(void)
{
	return notify_command;
}
const char *get_git_dir(void)
{
	return git_dir;
}
int get_sleep_secs(void)
{
	return sleep_secs;
}
