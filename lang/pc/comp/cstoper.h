/*  Copyright (c) 2019 ACK Project.
 *  See the copyright notice in the ACK home directory,
 *  in the file "Copyright".
 *
 */
#ifndef CSTOPER_H_
#define CSTOPER_H_

/* Forward struct declarations. */
struct node;

/*	The unary operation in "expp" is performed on the constant
	expression below it, and the result restored in expp.
*/
void cstunary(register struct node *expp);

/*	The binary operation in "expp" is performed on the constant
	expressions below it, and the result restored in expp.
*/
void cstbin(register struct node *expp);
void cstset(register struct node *expp);

/* Standard system function call that can be evaluated
 * a compile time.
 */
void cstcall(register struct node *expp, int req);

/* The constant value of the expression expr is made to conform
 * to the size of the type of the expression
 */
void CutSize(register struct node *expr);
void InitCst(void);



#endif /* CSTOPER_H_ */
