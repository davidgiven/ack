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

/* This is the main driving routine of the optimizer.
 * It first processes the flags given as argument;
 * for every flag it does not recognize itself, it
 * calls 'proc_flag'; as soon as the -M flag is seen,
 * it opens the machine descriptor file and
 * reads phase-independend information (notably the
 * wordsize and pointersize of the target machine);
 * next it calls 'phase_machinit' with this file as
 * parameter. Subsequently it calls 'initialize'.
 * Finally, all procedures are read, one at a time,
 * and 'optimize' is called with the current procedure
 * as parameter.
 */
void go(int argc, char * const *argv,
	void (*initialize)(void *null),
	void (*optimize)(void *),	/* (proc_p *p) */
	void (*phase_machinit)(void *),	/* (FILE *f) */
	void (*proc_flag)(void *));	/* (char *flag) */

/*
 * Parameter to be supplied for e.g. 'initialize' if
 * no action is required.
 */
void no_action(void *);

/* Report core usage, if core_flag is set. */
void core_usage(void);

/* Report number of optimizations found, if 
 * report_flag is set
 */
void report(char* s, int n);
