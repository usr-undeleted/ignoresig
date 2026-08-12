#include <asm-generic/errno-base.h>
#include <linux/limits.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <limits.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <libgen.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <time.h>

#include "utils.h"
#include "consts.h"
#include "colors.h"

// whenever enter key is pressed, stop execution

// set to argv[0]
const char *glob_invoc = NULL;
size_t  glob_invoc_len = 0;

bool print_msg  = true;
int  return_sig = 0;

void help(void) {
	fprintf(stderr,
		BOLD "%s" RESET " instructions:\n"
		"\n"

		BOLD "flags:\n" RESET
		BOLD "\t-h or --help" RESET ": show this help message.\n"
		BOLD "\t-n or --no-msg" RESET ": don't show a message when receiving a signal.\n"
		BOLD "\t-s or --signal <sig>" RESET ": send a specific signal to whoever sent the original signal.\n"
		"\n"

		BOLD "usage:\n" RESET
		"\t%s <signals> (flags may be specified, if needed)\n"
		"\n"

		"\tSignals to be specified may be any of the valid POSIX linux signals,"
		"where they will be ignored and shown a message (if enabled), showing who"
		" sent the signal, and what signal. A signal may be written as its number,"
		" name, or SIG directly followed by the name.\n"
		"\tThere's no case-sensitivity on signal arguments.\n"
		"\tClicking enter will end execution.\n"
		"\n"

		"ignoresig is licensed under GPL-V3, FOSS forever! <3\n"
		,

		basename((char *)glob_invoc),

		basename((char *)glob_invoc)

	);

	exit(0);
}

// get invocation from who sent signal
void populate_invoc_buffer(char *buf, size_t s, pid_t pid) {
	char path[PATH_MAX + 1] = {0};
	snprintf(path, sizeof(path), "/proc/%d/cmdline", pid);

	int fd = open(path, O_RDONLY);
	if (fd < 0) {
		set_uncheckable(buf, s);
		return;
	}

	memset(buf, '\0', s);

	if (!read(fd, buf, s)) {
		set_uncheckable(buf, s);
		return;
	}

	buf = basename(buf);
	*goto_whitespace(buf) = '\0';

	close(fd);
}

int self_kill_sig = -1;
int last_target   = -1;

void handler(int sig, siginfo_t *info, void *ucontext) {
	(void)ucontext;

	if (print_msg == true) {
		// form time
		char formatted_time[TIME_MAX] = {0};
		{
			time_t current_time;
			time(&current_time);

			struct tm *used_time = localtime(&current_time);

			snprintf(formatted_time, sizeof(formatted_time),
				"%s%d/%s%d/%s%d, (%s), %s%d:%s%d:%s%d",
				used_time->tm_mday < 10 ? "0" : "", used_time->tm_mday,
				used_time->tm_mon  < 10 ? "0" : "", used_time->tm_mon,
				used_time->tm_year + 1900 < 10 ? "0" : "", used_time->tm_year + 1900,
				weekdays[used_time->tm_wday],
				used_time->tm_hour < 10 ? "0" : "", used_time->tm_hour,
				used_time->tm_min  < 10 ? "0" : "", used_time->tm_min,
				used_time->tm_sec  < 10 ? "0" : "", used_time->tm_sec);
		}

		// get process invoc trough /proc/N/cmdline
		char signal_sender_name[CMD_MAX] = {0};
		{
			// if self
			if (info->si_pid == 0) {
				memcpy(signal_sender_name, glob_invoc,
					glob_invoc_len > CMD_MAX ? CMD_MAX : glob_invoc_len);

			} else {
				populate_invoc_buffer(signal_sender_name, sizeof(signal_sender_name),
					info->si_pid);
			}
		}

		int sender = info->si_pid == 0 ? access_own_pid() : info->si_pid;
		printf("[%.*s]: Received SIG%s (\x1b[3%dm%d\x1b[0m) from \x1b[3%dm%d\x1b[0m (%s)\n",
			(int)sizeof(formatted_time), formatted_time,
			sig_to_str(sig),
			sig    % 7 + 1, sig,
			sender % 7 + 1, sender,
			signal_sender_name);
	}

	if (return_sig) {
		// prevent recursive behaviour
		if (self_kill_sig == return_sig && last_target == access_own_pid()) {
			printf("%s: preventing recursive signal sending.\n", basename((char *)glob_invoc));
			self_kill_sig = -1;

		} else {
			self_kill_sig = return_sig;
			last_target   = info->si_pid == 0 ? access_own_pid() : info->si_pid;
			kill(info->si_pid, return_sig);
		}
	}
}

int set_return_signal(const char *argv[], const int i) {
	if (!argv[i + 1]) {
		fprintf(stderr, "%s: signal argument doesn't exist.\n",
			basename((char *)argv[0]));
		return 1;
	}

	if (!return_sig) {
		return_sig = arg_to_sig(argv[i + 1]);
		if (!return_sig) {
			fprintf(stderr, "%s: signal argument \"%s\" is not a valid signal.\n",
				basename((char *)argv[0]), argv[i + 1]);
		}
		return 1;

	} else {
		fprintf(stderr, "%s: signal argument will be ignored (has already been set).\n",
			basename((char *)argv[0]));
		return 1;
	}

	return 0;
}

int main (const int argc, const char *argv[]) {
	glob_invoc	   = argv[0];
	glob_invoc_len = strlen(argv[0]);
	bool fail = true;

	bool sig_enabled[signal_cnt];
	memset(sig_enabled, false, signal_cnt);

	bool sig_flag_used = false;

	for (int i = 1; i < argc; i++) {
		if (sig_flag_used == true) {
			sig_flag_used = false;
			continue;
		}

		// flags
		if (!strncmp(argv[i], "--", 2)) {
			// full flags
			if (!strcmp(argv[i], "--help")) {
				help();

			} else if (!strcmp(argv[i], "--no-msg")) {
				print_msg = false;

			} else if (!strcmp(argv[i], "--signal")) {
				if (set_return_signal(argv, i) == 0) {
					sig_flag_used = true;
				}
				i++;
				continue;

			} else {
				fprintf(stderr, "%s: invalid \"%s\" flag used.\nuse '-h' for help\n",
					basename((char *)argv[0]), argv[i]);
			}

		} else if (argv[i][0] == '-') {
			// single char flags
			size_t len = strlen(argv[i]);

			if (len < 2) {
				fprintf(stderr, "%s: a flag must be specified.\n", basename((char *)argv[0]));
				continue;
			}

			for (size_t j = 1; j < strlen(argv[i]); j++) {
				switch (argv[i][j]) {
					case 'h': {
						help();
						break;
					}

					case 'n': {
						print_msg = false;
						break;
					}

					case 's': {
						if (set_return_signal(argv, i) == 0) {
							sig_flag_used = true;
						}
						i++;
						continue;
						break;
					}

					default: {
						fprintf(stderr, "%s: invalid '%c' flag used.\nuse '-h' for help\n",
							basename((char *)argv[0]), argv[i][j]);
						break;
					}
				}
			}

		} else {
			bool minifail = true;

			int s = arg_to_sig(argv[i]);

			if (s != -1) {
				sig_enabled[s] = true;
				fail     = false;
				minifail = false;
			}

			if (minifail == true) {
				fprintf(stderr, "%s: invalid signal \"%s\".\n", basename((char *)argv[0]), argv[i]);
			}
		}
	}

	// set the signals
	struct sigaction sig = {0};
	sig.sa_sigaction = handler;
	sigemptyset(&sig.sa_mask);
	sig.sa_flags = SA_SIGINFO;

	for (size_t i = 1; i < sizeof(sig_enabled); i++) {
		if (sig_enabled[i] == false) continue;

		if (sigaction(signals[i].sig, &sig, NULL) != 0) {
			fprintf(stderr, "%s: failed to setup signal (%s).\n",
				basename((char *)argv[0]), strerror(errno));
		}
	}

	if (fail == true) {
		fprintf(stderr, "%s: no valid signal provided.\n",
			basename((char *)argv[0]));
		return 1;
	}

	while (1) {
		char i = 0;
		read(STDIN_FILENO, &i, 1);

		// ctrl + d
		if (i) {
			fprintf(stderr, "%s: aborting (user input)...\n", basename((char *)argv[0]));
			return 0;
		}
	}

	return 0;
}
