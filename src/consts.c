#include <signal.h>

#include "defs.h"

const signal_s signals[] = {
	{
		.string = "HUP",
		.sig	= SIGHUP,
	},

	{
		.string = "INT",
		.sig	= SIGINT,
	},

	{
		.string = "QUIT",
		.sig	= SIGQUIT,
	},

	{
		.string = "ILL",
		.sig	= SIGILL,
	},

	{
		.string = "TRAP",
		.sig	= SIGTRAP,
	},

	{
		.string = "ABRT",
		.sig	= SIGABRT,
	},

	{
		.string = "IOT",
		.sig	= SIGIOT,
	},

	{
		.string = "BUS",
		.sig	= SIGBUS,
	},

	/*
	{
		.string = "EMT",
		.sig	= SIGEMT,
	},
	*/

	{
		.string = "FPE",
		.sig	= SIGFPE,
	},

	{
		.string = "KILL",
		.sig	= SIGKILL,
	},

	{
		.string = "USR1",
		.sig	= SIGUSR1,
	},

	{
		.string = "SEGV",
		.sig	= SIGSEGV,
	},

	{
		.string = "USR2",
		.sig	= SIGUSR2,
	},

	{
		.string = "PIPE",
		.sig	= SIGPIPE,
	},

	{
		.string = "ALRM",
		.sig	= SIGALRM,
	},

	{
		.string = "TERM",
		.sig	= SIGTERM,
	},

	{
		.string = "STKFLT",
		.sig	= SIGSTKFLT,
	},

	{
		.string = "CHLD",
		.sig	= SIGCHLD,
	},

	{
		.string = "CLD",
		.sig	= SIGCLD,
	},

	{
		.string = "CONT",
		.sig	= SIGCONT,
	},

	{
		.string = "STOP",
		.sig	= SIGSTOP,
	},

	{
		.string = "TSTP",
		.sig	= SIGTSTP,
	},

	{
		.string = "TTIN",
		.sig	= SIGTTIN,
	},

	{
		.string = "TTOU",
		.sig	= SIGTTOU,
	},

	{
		.string = "URG",
		.sig	= SIGURG,
	},

	{
		.string = "XCPU",
		.sig	= SIGXCPU,
	},

	{
		.string = "XFSZ",
		.sig	= SIGXFSZ,
	},

	{
		.string = "VTALRM",
		.sig	= SIGVTALRM,
	},

	{
		.string = "PROF",
		.sig	= SIGPROF,
	},

	{
		.string = "WINCH",
		.sig	= SIGWINCH,
	},

	{
		.string = "IO",
		.sig	= SIGIO,
	},

	{
		.string = "POLL",
		.sig	= SIGPOLL,
	},

	{
		.string = "PWR",
		.sig	= SIGPWR,
	},

	/*
	{
		.string = "INFO",
		.sig	= SIGINFO,
	},
	*/

	/*
	{
		.string = "LOST",
		.sig	= SIGLOST,
	},
	*/

	{
		.string = "SYS",
		.sig	= SIGSYS,
	},

	/*
	{
		.string = "UNUSED",
		.sig	= SIGUNUSED,
	},
	*/
};

const size_t signal_cnt = sizeof(signals) / sizeof(signals[0]);

#ifndef SHORT_WEEKDAYS
#ifndef CAPITALIZED_WEEKDAYS
const char *weekdays[] = {
	"Sunday",
	"Monday",
	"Tuesday",
	"Wednesday",
	"Thursday",
	"Friday",
	"Saturday",
};

#else
const char *weekdays[] = {
	"SUNDAY",
	"MONDAY",
	"TUESDAY",
	"WEDNESDAY",
	"THURSDAY",
	"FRIDAY",
	"SATURDAY",
};

#endif // CAPITALIZED_WEEKDAYS

#else
#ifndef CAPITALIZED_WEEKDAYS
const char *weekdays[] = {
	"Sun",
	"Mon",
	"Tue",
	"Wed",
	"Thu",
	"Fri",
	"Sat",
};

#else
const char *weekdays[] = {
	"SUN",
	"MON",
	"TUE",
	"WED",
	"THU",
	"FRI",
	"SAT",
};

#endif // CAPITALIZED_WEEKDAYS
#endif // SHORT_WEEKDAYS
