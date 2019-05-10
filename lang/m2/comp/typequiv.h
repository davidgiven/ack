/*  Copyright (c) 2019 ACK Project.
 *  See the copyright notice in the ACK home directory, 
 *  in the file "Copyright".
 *
 *  Created on: 2019-02-25
 *  
 */
#ifndef TYPEQUIV_H_
#define TYPEQUIV_H_


struct node;
struct def;
struct type;

int TstProcEquiv(struct type *tp1, struct type *tp2);
int TstCompat(register struct type *tp1, register struct type *tp2);
int TstAssCompat(register struct type *tp1, register struct type *tp2);
int TstParCompat(int parno, register struct type *formaltype, int VARflag, struct node **nd, struct def *edf);

int ChkCompat(struct node **nd, struct type *tp, char *message);
int ChkAssCompat(struct node **nd, struct type *tp, char *message);

char *incompat(register struct type *tp1, register struct type *tp2);

#endif /* TYPEQUIV_H_ */
