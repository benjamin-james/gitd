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
		if (A == NULL) { \
			syslog(LOG_ERR, "Error in %s at %i in %s\n", __FILE__, __LINE__, __func__); \
			exit(EXIT_FAILURE); }}
#define check_less_zero(A) {   \
		if (A < 0) { \
			syslog(LOG_ERR, "Error in %s at %i in %s\n", __FILE__, __LINE__, __func__); \
			exit(EXIT_FAILURE); }}

void gitd_loop(void)
{
	gitd_check_reps();
	gitd_notify();
}

void gitd_init(void)
{
	pid_t pid = fork();
	int x;
	if (pid < 0)
		exit(EXIT_FAILURE);
	if (pid > 0)
		exit(EXIT_SUCCESS);
	if (setsid() < 0)
		exit(EXIT_FAILURE);
	umask(0);
	chdir("/");
	for (x = sysconf(_SC_OPEN_MAX); x > 0; x--)
		close(x);
	openlog(NULL, LOG_PID, LOG_DAEMON);
	if (config_load("/etc/gitd/gitd.conf") < 0) {
		syslog(LOG_ERR, "Could not load /etc/gitd/gitd.conf");
		exit(EXIT_FAILURE);
	}
}

void gitd_git_dirs(struct gitd *array)
{
	DIR *dir = opendir(get_git_dir());
	struct dirent *entry = NULL;
	char buffer[256];
	struct stat st;
	if (dir == NULL) {
		syslog(LOG_ERR, "Could not open directory \"%s\"\n", get_git_dir());
		exit(EXIT_FAILURE);
	}
	while (entry = readdir(dir)) {
		if (!strcmp(entry->d_name, "..") || !strcmp(entry->d_name, ".") || stat(entry->d_name, &st) != 0 || !(S_ISDIR(st.st_mode)))
			continue;
		sprintf(buffer, "%s/%s/.gitd", get_git_dir(), entry->d_name);
		if (gitd_fetch(buffer) > 0)
			gitd_notify(entry->d_name);
	}
	closedir(d);
}
/*
 * Check to see if the repository is
 * updated. If it is, download the diff
 * and return 1.
 * Otherwise, return 0
 */
int gitd_fetch(const char *path)
{
	git_repository *repository;
	git_remote *remote;
	git_revwalk *revwalk;
	git_commit *commit;
	git_oid id;
	git_repository_open(&repository, path);
	git_remote_load(&remote, repository, "origin");
	git_remote_fetch(remote, NULL, NULL, NULL);

/*	git_revwalk_new(&revwalk, repository);
	git_revwalk_next(&id, revwalk);
	git_commit_lookup(&commit, repository, &id);
	gitd_notify(commit);
	git_revwalk_free(revwalk);*/
}
/*
 * Use notify_command to send
 * the commit, replace all special
 * characters with the corresponding
 * commit info
 */
void gitd_notify(git_commit *commit)
{
	char buffer[256];
	strcpy(buffer, get_notify_command());

}
