/*
 * VideoCore IV assembler for the ACK
 * © 2013 David Given
 * This file is redistributable under the terms of the 3-clause BSD license.
 * See the file 'Copying' in the root of the distribution for the full text.
 */

#define	THREE_PASS          /* branch and offset optimization */
#define LISTING             /* enable listing facilities */
#define RELOCATION          /* generate relocatable code */
#define DEBUG 0

#undef valu_t
#define valu_t long

#undef ADDR_T
#define ADDR_T long

#undef word_t
#define word_t long

typedef unsigned long quad;

#undef ALIGNWORD
#define ALIGNWORD	4

#undef ALIGNSECT
#define ALIGNSECT	4

#undef VALWIDTH
#define VALWIDTH	8

#define FIXUPFLAGS (RELBR | RELWR)
