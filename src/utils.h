#ifndef UTILS_H
#define UTILS_H

#include <sys/types.h>
#define UNKNOWN "<unknown>"

// match sig num to string
// return NULL on error
char *sig_to_str(int sig);

// match string to sig num
int   str_to_sig(const char *str);

// don't run getpid() twice
pid_t access_own_pid(void);

#define UNCHECKABLE "<uncheckable>"

// clear a buffer and set it to uncheckable macro
void set_uncheckable(char *buf, size_t s);

// move pointer to whitespace
char *goto_whitespace(const char *ptr);

#endif
