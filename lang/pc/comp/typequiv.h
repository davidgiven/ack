/*  Copyright (c) 2019 ACK Project.
 *  See the copyright notice in the ACK home directory,
 *  in the file "Copyright".
 *
 */
#ifndef TYPEQUIV_H_
#define TYPEQUIV_H_

#include	"em_arith.h"


struct type;
struct node;

/*	test if two types are equivalent. */
int TstTypeEquiv(struct type *tp1, struct type *tp2);
arith IsString(struct type *tp);
/*	test if two types are compatible string-types. */
int TstStrCompat(struct type *tp1, struct type *tp2);
/*	test if two types are compatible. ISO 6.4.5 */
int TstCompat(struct type *tp1, struct type *tp2);
/*	test if two types are assignment compatible. ISO 6.4.6 */
int TstAssCompat(struct type *tp1, struct type *tp2);
/*	Test if two parameter types are equivalent.  ISO 6.6.3.6 */
int TstParEquiv(struct type *tp1, struct type *tp2);
/*	Test if two procedure types are equivalent. ISO 6.6.3.6 */
int TstProcEquiv(struct type *tp1, struct type *tp2);
/*	Check type compatibility for a parameter in a procedure call. */
int TstParCompat(struct type *formaltype, struct type *actualtype,
	int VARflag, struct node *nd, int new_par_section);
/*	Check conformability.

	DEVIATION FROM STANDARD (ISO 6.6.3.7.2):
	Allow with value parameters also conformant arrays as actual
	type.(ISO only with var. parameters)

	Do as much checking on indextypes as possible.
*/
int TstConform(struct type *formaltype, struct type * actualtype, int new_par_section);


#endif /* TYPEQUIV_H_ */
