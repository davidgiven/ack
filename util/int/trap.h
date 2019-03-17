/*
	Trap handling
*/
#ifndef TRAP_H_
#define TRAP_H_

#include	"warn.h"

/* $Id$ */

#define	wtrap(wn,tr)	(warning(wn), trap(tr))
#define	trap(tr)	do_trap(tr, __LINE__, __FILE__)

extern int signalled;			/* signal nr if trap was due to sig */

extern int must_test;			/* must trap on overfl./out of range*/
					/* TEST-bit on in EM header word 2 */

/* Execute the specified trap. "nr" represents the signal
 * number, "L" is the line number and "F" is the filename
 * where the trap occurred.
 */
void do_trap(int nr, int L, char *F);

#endif /* TRAP_H_ */


