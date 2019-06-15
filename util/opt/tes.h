/*
 * Author: Hans van Eck. 
 */
#ifndef TES_H_
#define TES_H_

#include "types.h"

extern int state;
#define	KNOWN		1
#define	NOTREACHED	2

void init_state(void);
void tes_pseudos(void);
void tes_instr(line_p lnp, line_p x, line_p y);

#endif /* TES_H_ */
