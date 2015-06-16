#ifndef SETTINGS_H
#define SETTINGS_H

const char *get_notify_command(void);
const char *get_git_dir(void);
int get_sleep_secs(void);

int load_config(const char *filepath);
#endif
