#ifndef _SYS_TIMES_H
#define _SYS_TIMES_H

struct tms {
    clock_t tms_utime;  /* user time */
    clock_t tms_stime;  /* system time */
    clock_t tms_cutime; /* user time of children */
    clock_t tms_cstime; /* system time of children */
};

extern clock_t times(struct tms* buf);

#endif
