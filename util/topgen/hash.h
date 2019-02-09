/*
 * hash.h
 *
 *  Created on: 2018-11-24
 *      Author: carl
 */

#ifndef __HASH_H_INCLUDED__
#define __HASH_H_INCLUDED__

/*
 * Add a new pattern number to the hashtable.
 * s is the key, n the pattern number
 */
void addtohashtable(char* s, int n);
/*
 * Print the linear lists, and also output an array of
 * pointers to them
 */
void printhashtable(void);


#endif /* __HASH_H_INCLUDED__ */
