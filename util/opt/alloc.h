/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Header$ */

extern line_p 	newline();
extern offset	*newrom();
extern sym_p	newsym();
extern num_p	newnum();
extern arg_p	newarg();
extern argb_p	newargb();
extern reg_p	newreg();

extern		oldline();
extern		oldloc();
extern		oldreg();

#undef USEMALLOC	/* if defined malloc() and free() are used */

#undef COREDEBUG	/* keep records and print statistics */

/*
 * The next define gives if defined the number of pseudo's outside
 * procedures that are collected without processing.
 * If undefined all pseudo's will be collected but that may
 * give trouble on small machines, because of lack of room.
 */
#define PSEUBETWEEN 200 

#ifndef USEMALLOC
/*
 * Now the real bitsqueezing starts.
 * When running on a machine where code and data live in
 * separate address-spaces it is worth putting in some extra
 * code to save on probably less data.
 */
#define SEPID		/* code and data in separate spaces */
/*
 * If the stack segment and the data are separate as on a PDP11 under UNIX
 * it is worth squeezing some shorts out of the stack page.
 */
#define STACKROOM 3200	/* number of shorts space in stack */

#else

#define STACKROOM 1	/* 0 gives problems */

#endif	/* USEMALLOC */
