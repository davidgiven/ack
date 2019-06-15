/*
 * extract.h
 *
 *  Created on: 2018-11-17
 *      Author: carl
 */

#ifndef __EXTRACT_H_INCLUDED__
#define __EXTRACT_H_INCLUDED__

#include "out.h"

void extract(void);
/*
 * Add relocation constant for names in user defined sections.
 * The value of a common name indicates a size instead of an offset,
 * and hence shouldn't be relocated.
 * Otherwise we just add the accumulated size of all normal parts in preceding
 * sections with the same size.
 */
void namerelocate(register struct outname *name);

#endif /* __EXTRACT_H_INCLUDED___ */
