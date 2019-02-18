/*
 * pattern.h
 *
 *  Created on: 2018-11-25
 *      Author: carl
 */

#ifndef __PATTERN_H_INCLUDED__
#define __PATTERN_H_INCLUDED__


/*
 * Just add a pattern to the list.
 * "str" is the constraint, "l" is the line number,
 * "np" is the number of instructions in the pattern,
 * "nr" is the number of instructions in the replacement
 * Space is allocated in chunks of 50
 */
void addpattern(char* str,int l,int np,int nr);

/*
 * Prints the pattern_descr table and generates the routine
 * "check_constraint"
 */
void printpatterns(void);

#endif /* __PATTERN_H_INCLUDED__ */
