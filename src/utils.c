#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <ctype.h>

#include "utils.h"

char *sig_to_str(int sig) {
    switch (sig) {
        case SIGHUP:    {return    "HUP"; break;}
        case SIGINT:    {return    "INT"; break;}
        case SIGQUIT:   {return   "QUIT"; break;}
        case SIGILL:    {return    "ILL"; break;}
        case SIGTRAP:   {return   "TRAP"; break;}
        case SIGABRT:   {return   "ABRT"; break;}
        // case SIGIOT:    {return    "IOT"; break;}
        case SIGBUS:    {return    "BUS"; break;}
        // case SIGEMT:    {return    "EMT"; break;}
        case SIGFPE:    {return    "FPE"; break;}
        case SIGUSR1:   {return   "USR1"; break;}
        case SIGSEGV:   {return   "SEGV"; break;}
        case SIGUSR2:   {return   "USR2"; break;}
        case SIGPIPE:   {return   "PIPE"; break;}
        case SIGALRM:   {return   "ALRM"; break;}
        case SIGTERM:   {return   "TERM"; break;}
        case SIGSTKFLT: {return "STKFLT"; break;}
        case SIGCHLD:   {return   "CHLD"; break;}
        // case SIGCLD:    {return    "CLD"; break;}
        case SIGCONT:   {return   "CONT"; break;}
        case SIGSTOP:   {return   "STOP"; break;}
        case SIGTSTP:   {return   "TSTP"; break;}
        case SIGTTIN:   {return   "TTIN"; break;}
        case SIGTTOU:   {return   "TTOU"; break;}
        case SIGURG:    {return    "URG"; break;}
        case SIGXCPU:   {return   "XCPU"; break;}
        case SIGXFSZ:   {return   "XFSZ"; break;}
        case SIGVTALRM: {return "VTALRM"; break;}
        case SIGPROF:   {return   "PROF"; break;}
        case SIGWINCH:  {return  "WINCH"; break;}
        case SIGIO:     {return     "IO"; break;}
        // case SIGPOLL:   {return   "POLL"; break;}
        case SIGPWR:    {return    "PWR"; break;}
        // case SIGINFO:   {return   "INFO"; break;}
        // case SIGLOST:   {return   "LOST"; break;}
        case SIGSYS:    {return    "SYS"; break;}
        // case SIGUNUSED: {return "UNUSED"; break;}

    }

    return "<unknown>";
}

pid_t global_own_pid = -1;

pid_t access_own_pid(void) {
    if (global_own_pid == -1) global_own_pid = getpid();
    return global_own_pid;
}

void set_uncheckable(char *buf, size_t s) {
    memset(buf, '\0', s);
    memcpy(buf, UNCHECKABLE, sizeof(UNCHECKABLE) > s ? s : sizeof(UNCHECKABLE));
}

char *goto_whitespace(const char *ptr) {
    char *ret = (char *)ptr;

    while (*ret && !isspace(*ret)) {
        ret++;
    }

    return ret;
}
