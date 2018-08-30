/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#ifdef VERBOSE

extern void SHOWOCCUR(occur_p ocp);
			/*
			 * Shows all lines in an occurrence.
			 */

#else

#define SHOWOCCUR(x)

#endif

#ifdef TRACE

extern void OUTAVAILS(void);
			/*
			 * Prints all available expressions.
			 */

extern void OUTENTITIES(void);
			/*
			 * Prints all entities.
			 */

extern void SHOWAVAIL(avail_p avp);
			/*
			 * Shows an available expression.
			 */

#else /* TRACE */

#define OUTAVAILS()
#define OUTENTITIES()
#define SHOWAVAIL(x)

#endif /* TRACE */
