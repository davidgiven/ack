#ifndef SYM_H
#define SYM_H

extern unsigned short NLocals;	/* Number of local names to be saved. */
extern unsigned short NGlobals;	/* Number of global names. */

extern void entername(struct outname* name, int hashval);

#endif
