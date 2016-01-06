#ifndef GITD_H
#define GITD_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <string.h>
#include <unistd.h>

int loop(void);
int repo(const char *dir_to_check);

int is_file(const char *path);
int is_dir(const char *path);
int setup_reaper(void);
void sigchld_handler(int signum);
int do_for_repo(const char *dir_to_check);
int do_for_loop(void);

int is_git_directory(const char *path);
int check_git_repo(void);
int notify(void);

int init(int argc, char *const *argv);
int print_help(void); /* TODO */
int get_input(int argc, char *const *argv);
int get_option(int character);
int load_config(void);

int try_set_env_value(const char *env, const char *value);
int set_default_envvars(const char *argv_0);

int open_log(void);
int lputs(const char *str);
int lperror(const char *str);
int close_log(void);

#define LOG_BUF_STRLEN 2048
char _log_str[LOG_BUF_STRLEN];
char _log_buf[LOG_BUF_STRLEN];

#define lprintf(FORMAT, ...) { \
	snprintf(_log_str, LOG_BUF_STRLEN, "[%ld] Line %d in %s in %s: ", (long)getpid(), __LINE__, __func__, __FILE__); \
	snprintf(_log_buf, LOG_BUF_STRLEN, FORMAT, ##__VA_ARGS__); \
	strncat(_log_str, _log_buf, LOG_BUF_STRLEN); \
	lputs(_log_str); }
#endif
