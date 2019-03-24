/*
 * output.h
 *
 *  Created on: 2018-11-18
 *      Author: carl
 */

#ifndef __OUTPUT_H_INCLUDED__
#define __OUTPUT_H_INCLUDED__

/*
 * We have counted all relocation structs but we know only now if
 * these must be emitted.We add all names here,unless the -s(trip)
 * flag was given.
 * If this flag is given we don't need the string table either.
 */
void beginoutput(void);

/*
 * If we didn't keep the whole output file in core, most of it has been
 * written out, and we just finish that.
 * If we did, we write out our pieces of core.
 */
void endoutput(void);


#endif /* __OUTPUT_H_INCLUDED__ */
