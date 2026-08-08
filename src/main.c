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

// sleep indefinitely, and ignore the signals specified by the user

// set to argv[0]
const char *glob_invoc = NULL;
size_t  glob_invoc_len = 0;

bool print_msg = 1;

void help(void) {
    fprintf(stderr,
        "%s instructions:\n\n"

        "flags:\n"
        "\t-h: show this help message.\n"
        "\t-n: don't show a message when receiving a signal.\n\n"

        "usage:\n"
        "\t%s <signals> (flags may be specified, if needed)\n\n"

        "\tsignals to be specified may be any of the valid POSIX linux signals,"
        "where they will be ignored and shown a message (if enabled), showing who"
        " sent the signal, and what signal. A signal may be written as its number,"
        " name, or SIG followed by the name. There's no case-sensitivity.\n"
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

        printf("[%.*s]: Received SIG%s (%d) from %d (%s)\n", (int)sizeof(formatted_time), formatted_time,
            sig_to_str(sig), sig, info->si_pid == 0 ? access_own_pid() : info->si_pid, signal_sender_name);
    }
}

void error(const char *msg, const bool use_errno) {
    fprintf(stderr, "%s: %s%s%s%s", basename((char *)glob_invoc), msg,
        use_errno == true ? " (" : "", use_errno == true ? strerror(errno) : "", use_errno == true ? ").\n" : ".\n");

    exit(1);
}

int main (const int argc, const char *argv[]) {
    glob_invoc     = argv[0];
    glob_invoc_len = strlen(argv[0]);
    bool fail = true;

    bool sig_enabled[signal_cnt];
    memset(sig_enabled, false, signal_cnt);

    help();

    for (int i = 1; i < argc; i++) {
        // flags
        {
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

                    default: {
                        char msg[48] = {0};
                        snprintf(msg, sizeof(msg), "invalid '%c' flag used.\nuse '-h' for help",
                            argv[i][j]);
                        error(msg, false);
                        break;
                    }
                }
            }
        }

        bool minifail = true;

        // numbers
        {
            char *p = NULL;
            size_t n = strtol(argv[i], &p, 0);

            if ((n >= 1 && n <= signal_cnt) && !*p) {
                sig_enabled[n] = true;
                minifail       = false;
                fail           = false;
                continue;
            }
        }

        // regular strings
        {
            // offset by sizeof("sig")
            size_t off = 0;
            if (!strncasecmp(argv[i], "SIG", 3)) off += 3;

            for (size_t j = 0; j < signal_cnt; j++) {

                if (!strcasecmp(signals[j].string, argv[i] + off)) {
                    sig_enabled[j] = true;
                    minifail       = false;
                    fail           = false;
                    break;
                }
            }
        }

        if (minifail == true) {
            fprintf(stderr, "%s: invalid signal \"%s\".\n", basename((char *)argv[0]), argv[i]);
        }
    }

    // set the signals
    {
        struct sigaction sig = {0};
        sig.sa_sigaction = handler;
        sigemptyset(&sig.sa_mask);
        sig.sa_flags = SA_SIGINFO;

        for (size_t i = 0; i < sizeof(sig_enabled); i++) {
            if (sig_enabled[i] == true) {

                if (sigaction(signals[i].sig, &sig, NULL) != 0) {
                    error("failed to setup signal", true);
                }
            }
        }
    }

    if (fail == true) error("no valid signal found", false);

    while (1) sleep(999);

    return 0;
}
