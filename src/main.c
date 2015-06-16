#include "gitd.h"
#include "settings.h"
#include "unistd.h"

int main(int argc, char **argv)
{
	gitd_init();
	while (1) {
		gitd_loop();
		sleep(get_sleep_secs());
	}
}
