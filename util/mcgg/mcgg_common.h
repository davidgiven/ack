#ifndef MCGG_COMMON_H
#define MCGG_COMMON_H

/* Excruciating macro which packs ir opcodes and sizes into an int for iburg's benefit.
 *
 * Types are mapped to: I=1, F=2, L=3, D=4
 */
#define ir_to_esn(iropcode, type) \
	((iropcode)*5 + \
		(((type) == 'I') ? 1 : \
		 ((type) == 'F') ? 2 : \
		 ((type) == 'L') ? 3 : \
         ((type) == 'D') ? 4 : 0))

enum
{
    NONTERM_STMT = 1
};

enum
{
	INSN_STMT = 1
};

#endif

