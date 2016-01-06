#include <getopt.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gitd.h"

#define CONFIG_ENVVAR_STRLEN 20
#define NUM_CONFIG_ENVVARS 9

/*
 * Environment variables that could be
 * set by the config file.
 * Other environment variables are
 * PROGRAM_NAME and CONFIG_FILE.
 */
static const char config_envvars[][CONFIG_ENVVAR_STRLEN] = {"DIR", "SLEEP", "NOTIFY", "GIT_CHECK", "FUNCTION", "FORKING", "FOREACH_REPO", "FOREACH_LOOP", "LOG_FILE"};

int print_help(void)
{
	const char *program_name = getenv("PROGRAM_NAME");
	printf("Usage: %s [OPTIONS]\n", program_name);
	return 0;
}

int get_input(int argc, char *const *argv)
{
	int character, option_index, ret = 0;
	static struct option long_options[] = {
		{"config", required_argument, 0, 'c'},
		{"dir", required_argument, 0, 'd'},
		{"git-check", required_argument, 0, 'g'},
		{"help", no_argument, 0, 'h'},
		{"notify", required_argument, 0, 'n'},
		{"sleep", required_argument, 0, 's'},
		{"quiet", no_argument, 0, 'q'},
		{"log-file", required_argument, 0, 'e'},

		{"function", required_argument, 0, 'u'},
		{"oneshot", no_argument, 0, 'o'},
		{"loop", no_argument, 0, 'l'},

		{"fork", optional_argument, 0, 'f'},
		{"no-fork", no_argument, 0, 'x'},

		{"for-each-loop", required_argument, 0, 'p'},
		{"for-each-repo", required_argument, 0, 'r'},

		{"fork-for-each", optional_argument, 0, 'v'},
		{"thread-for-each", optional_argument, 0, 't'},
		{0, 0, 0, 0}
	};
	while (!ret && (character = getopt_long(argc, argv, "c:d:g:hn:s:qe:u:olf::xp:r:v::t::", long_options, &option_index)) != -1) {
		ret = get_option(character);
	}
	while (optind < argc) {
		printf("Unrecognized option \"%s\"\n", argv[optind++]);
		ret = -1;
	}
	return ret;
}

int init(int argc, char *const *argv)
{
	int ret = 0;
	if (set_default_envvars(*argv) < 0) {
		printf("Error: Could not set default environment variables.\n");
		return -1;

	} else if (is_file(getenv("CONFIG_FILE")) == 1) {
		if (load_config() < 0) {
			return -1;
		}
	} else {
		printf("Warning: Config file %s does not exist, so it can't be loaded.\n", getenv("CONFIG_FILE"));
	}
	if (get_input(argc, argv) < 0) {
		ret = -1;
	} else if (open_log() < 0) {
		ret = -1;
	} else if (chdir(getenv("DIR")) < 0) {
		lperror("Could not change directory");
		ret = -1;
	}
	ret = setup_reaper();
	return ret;
}
/*
 * Parses each character returned from getopt_long(3)
 *
 * Returns 0 if the program can run
 */
int get_option(int character)
{
	char *endptr = NULL;
	long l;
	int ret = 0;
	switch (character) {
	case 'c': { /* config */
		if (!is_file(optarg)) {
			printf("Error: \"%s\" is not a valid file.\n", optarg);
			ret = -1;
		} else {
			char real_path[PATH_MAX];
			if (!realpath(optarg, real_path)) {
				snprintf(real_path, PATH_MAX, "Could not open %s", optarg);
				perror(real_path);
				ret = -1;
				break;
			}
			ret = try_set_env_value("CONFIG_FILE", real_path);
			if (!ret) {
				ret = load_config();
			}
		}
		break;
	}
	case 'd': { /* dir */
		if (!is_dir(optarg)) {
			printf("Error: \"%s\" is not a valid directory.\n", optarg);
			ret = -1;
		} else {
			ret = try_set_env_value("DIR", optarg);
		}
		break;
	}
	case 'g': { /* git-check */
		ret = try_set_env_value("GIT_CHECK", optarg);
		break;
	}
	case 'h': { /* help */
		ret = -1;
		print_help();
		break;
	}
	case 'n': { /* notify */
		ret = try_set_env_value("NOTIFY", optarg);
		break;
	}
	case 's': { /* sleep */
		l = strtol(optarg, &endptr, 10);
		if (l > UINT_MAX || l <= 0 || *endptr) {
			printf("Please enter a positive integer for sleep time.\n");
			ret = -1;
		} else {
			ret = try_set_env_value("SLEEP", optarg);
		}
		break;
	}
	case 'q': { /* quiet */
		ret = try_set_env_value("QUIET", "TRUE");
		break;
	}
	case 'e': { /* log-file */
		ret = try_set_env_value("LOG_FILE", optarg);
		break;
	}
	case 'u': { /* function */
		if (strcmp(optarg, "oneshot") && strcmp(optarg, "loop")) {
			printf("Please enter a valid choice for 'function'.\n");
			ret = -1;
		} else {
			ret = try_set_env_value("FUNCTION", optarg);
		}
		break;
	}
	case 'o': { /* oneshot */
		ret = try_set_env_value("FUNCTION", "oneshot");
		break;
	}
	case 'l': { /* loop */
		ret = try_set_env_value("FUNCTION", "loop");
		break;
	}
	case 'f': { /* fork */
		if (optarg) {
			if (strcasecmp(optarg, "true") && strcasecmp(optarg, "false")) {
				printf("Please enter a valid option for 'fork'\n");
				ret = -1;
			} else {
				ret = try_set_env_value("FORKING", optarg);
			}
		} else {
			ret = try_set_env_value("FORKING", "TRUE");
       		}
		break;
	}
	case 'x': { /* no-fork */
		ret = try_set_env_value("FORKING", "FALSE");
		break;
	}
	case 'p': { /* for-each-loop */
		if (strcasecmp(optarg, "none") && strcasecmp(optarg, "fork") && strcasecmp(optarg, "thread")) {
			printf("Please enter a valid option for 'for-each-loop'.\n");
			ret = -1;
		} else {
			ret = try_set_env_value("FOREACH_LOOP", optarg);
		}
		break;
	}
	case 'r': { /* for-each-repo */
		if (strcasecmp(optarg, "none") && strcasecmp(optarg, "fork") && strcasecmp(optarg, "thread")) {
			printf("Please enter a valid option for 'for-each-repo'.\n");
			ret = -1;
		} else {
			ret = try_set_env_value("FOREACH_REPO", optarg);
		}
		break;
	}
	case 'v': { /* fork-for-each */
		if (optarg) {
			if (!strcasecmp(optarg, "loop")) {
				ret = try_set_env_value("FOREACH_LOOP", "fork");
			} else if (!strcasecmp(optarg, "repo")) {
				ret = try_set_env_value("FOREACH_REPO", "fork");
			} else {
				printf("Please enter a valid option for 'fork-for-each'.\n");
				ret = -1;
			}
		} else {
			ret = try_set_env_value("FOREACH_LOOP", "fork");
			if (!ret) {
				ret = try_set_env_value("FOREACH_REPO", "fork");
			}
		}
		break;
	}
	case 't': { /* thread-for-each */
		if (optarg) {
			if (!strcasecmp(optarg, "loop")) {
				ret = try_set_env_value("FOREACH_LOOP", "thread");
			} else if (!strcasecmp(optarg, "repo")) {
				ret = try_set_env_value("FOREACH_REPO", "thread");
			} else {
				printf("Please enter a valid option for 'thread-for-each'.\n");
				ret = -1;
			}
		} else {
			ret = try_set_env_value("FOREACH_LOOP", "thread");
			if (!ret) {
				ret = try_set_env_value("FOREACH_REPO", "thread");
			}
		}
		break;
	}
	default: {
		ret = -1;
		print_help();
		break;
	}
	}
	return ret;
}

/*
 * Finds the envvar being set in this line from the output
 * of the config
 */
int config_set_envar(const char *line, const char *value)
{
	int i;
	char *str = malloc(CONFIG_ENVVAR_STRLEN + 1);
	if (!str) {
		perror("malloc");
		return -1;
	}
	for (i = 0; i < NUM_CONFIG_ENVVARS; i++) {
		int env_len = strlen(config_envvars[i]);
		memcpy(str, config_envvars[i], env_len + 1);
		if (env_len < CONFIG_ENVVAR_STRLEN) {
			str[env_len++] = '=';
			str[env_len] = '\0';
		}
		if (strstr(line, str)) {
			setenv(config_envvars[i], value, 1);
			free(str);
			return 0;
		}
	}
	free(str);
	return -1;
}
int get_lines(FILE *f)
{
	size_t num_bytes = 0, v_len;
	ssize_t line_len;
	char *line = NULL, *value = NULL;
	while ((line_len = getline(&line, &num_bytes, f)) != -1) {
		value = strchr(line, '=');
		if (!value || !++value) {
			continue;
		}
		v_len = strlen(value);
		if (value[v_len - 1] == '\n') {
			value[--v_len] = '\0';
		}
		if (config_set_envar(line, value) < 0) {
			fprintf(stderr, "Error parsing config: %s", line);
			return -1;
		}
	}
	return 0;
}
int load_config(void)
{
	const char format[] = ". %s; echo DIR=$DIR; echo SLEEP=$SLEEP; echo NOTIFY=$NOTIFY; echo GIT_CHECK=$GIT_CHECK; echo FUNCTION=$FUNCTION; echo FORKING=$FORKING; echo FOREACH_REPO=$FOREACH_REPO; echo FOREACH_LOOP=$FOREACH_LOOP; echo LOG_FILE=$LOG_FILE";
	size_t buf_len;
	char *buffer = NULL;
	const char *config_file = getenv("CONFIG_FILE");
	FILE *f;
	if (!config_file || is_file(config_file) != 1) {
		printf("Error: Could not open file %s.\n", config_file);
		return -1;
	}
	buf_len = strlen(format) + strlen(config_file);
	buffer = malloc(buf_len);
	if (!buffer) {
		perror("malloc");
		return -1;
	}
	if (snprintf(buffer, buf_len, format, config_file) < 0) {
		perror("snprintf");
		free(buffer);
		return -1;
	}
	f = popen(buffer, "r");
	free(buffer);
	if (!f) {
		perror("popen");
		return -1;
	}
	if (get_lines(f) < 0) {
		return -1;
	}
	if (pclose(f) < 0) {
		perror("pclose");
		return -1;
	}
	return 0;
}
/*
int main(int argc, char *argv[])
{
	int i, ret = init(argc, argv);
	if (ret < 0) {
		return 1;
	}
	for (i = 0; i < NUM_CONFIG_ENVVARS; i++) {
		printf("%s=%s\n", config_envvars[i], getenv(config_envvars[i]));
	}
	printf("PROGRAM_NAME=%s\n", getenv("PROGRAM_NAME"));
	printf("CONFIG_FILE=%s\n", getenv("CONFIG_FILE"));
	printf("QUIET=%s\n", getenv("QUIET"));
	return 0;
}
*/
