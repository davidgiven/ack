/*
 * relocate.h
 *
 *  Created on: 2018-11-18
 *      Author: carl
 */

#ifndef __RELOCATE_H_INCLUDED__
#define __RELOCATE_H_INCLUDED__

#include "out.h"

/*
 * This routine relocates a value in a section pointed to by `emit', of
 * which the header is pointed to by `head'. Relocation is relative to the
 * names in `names'; `relo' tells how to relocate.
 */
void relocate(struct outhead *head, char* emit, struct outname names[], struct outrelo *relo, long off);


#endif /* __RELOCATE_H_INCLUDED__ */
