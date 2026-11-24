/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/*  S H A R E D    F I L E
 *
 *  G O . H
 *
 */

#include <libconfig.h>

/* This is the main driving routine of the optimizer.
 * It first processes the flags given as argument;
 * for every flag it does not recognize itself, it
 * calls 'proc_flag'. After processing the flags, it
 * parses the -M machine description with libconfig and
 * reads the target word and pointer sizes. It then calls
 * 'phase_machinit' with a config_t pointer, which is valid
 * only for the duration of that callback. The configuration
 * is destroyed before calling 'initialize'.
 * Finally, all procedures are read, one at a time,
 * and 'optimize' is called with the current procedure
 * as parameter.
 */
void go(
    int argc,
    char* const* argv,
    void (*initialize)(void* null),
    void (*optimize)(void*), /* (proc_p *p) */
    void (*phase_machinit)(config_t*),
    void (*proc_flag)(void*)); /* (char *flag) */

/*
 * Parameter to be supplied for e.g. 'initialize' if
 * no action is required.
 */
void no_action(void*);

/* Machine initialization callback for phases with no machine settings. */
void no_machinit(config_t*);

/* Report core usage, if core_flag is set. */
void core_usage(void);

/* Report number of optimizations found, if
 * report_flag is set
 */
void report(char* s, int n);
