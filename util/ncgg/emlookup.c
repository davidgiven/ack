/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#ifndef NORCSID
static char rcsid[]= "$Id$";
#endif

#include "param.h"
#include "expr.h"
#include <em_spec.h>
#include <em_flag.h>

extern char em_mnem[][4];

#define HASHSIZE        (2*(sp_lmnem-sp_fmnem))

struct emhashmnem {
	char h_name[3];
	char h_value;
} emhashmnem[HASHSIZE];

initemhash() {
	register i;

	for(i=0;i<=sp_lmnem-sp_fmnem;i++)
		enter(em_mnem[i],i+sp_fmnem);
	enter("lab", op_lab);
}

unsigned emhash(name) register char *name; {
	register unsigned sum;
	register i;

	for (sum=i=0;*name;i+=3)
		sum ^= (*name++)<<(i&07);
	return(sum);
}

enter(name,value) char *name; {
	register unsigned h;

	h=emhash(name)%HASHSIZE;
	while (emhashmnem[h].h_name[0] != 0)
		h = (h+1)%HASHSIZE;
	strncpy(emhashmnem[h].h_name,name,3);
	emhashmnem[h].h_value = value;
}

int mlookup(name) char *name; {
	register unsigned h;

	h = emhash(name)%HASHSIZE;
	while (strncmp(emhashmnem[h].h_name,name,3) != 0 &&
	       emhashmnem[h].h_name[0] != 0)
		h = (h+1)%HASHSIZE;
	return(emhashmnem[h].h_value&0xFF);      /* 0 if not found */
}

extern char em_flag[];

argtyp(mn) {

	switch(em_flag[mn-sp_fmnem]&EM_PAR) {
	case PAR_W:
	case PAR_S:
	case PAR_Z:
	case PAR_O:
	case PAR_N:
	case PAR_L:
	case PAR_F:
	case PAR_R:
	case PAR_C:
		return(TYPINT);
	default:
		return(TYPADDR);
	}
}
