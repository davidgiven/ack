/*  Copyright (c) 2019 ACK Project.
 *  See the copyright notice in the ACK home directory, 
 *  in the file "Copyright".
 *
 *  Created on: 2019-02-25
 *  
 */
#ifndef TYPEQUIV_H_
#define TYPEQUIV_H_

typedef struct type t_type;
typedef struct node t_node;
typedef struct def  t_def;

int TstProcEquiv(t_type *tp1, t_type *tp2);
int TstCompat(register t_type *tp1, register t_type *tp2);
int TstAssCompat(register t_type *tp1, register t_type *tp2);
int TstParCompat(int parno, register t_type *formaltype, int VARflag, t_node **nd, t_def *edf);

int ChkCompat(t_node **nd, t_type *tp, char *message);
int ChkAssCompat(t_node **nd, t_type *tp, char *message);

char *incompat(register t_type *tp1, register t_type *tp2);

#endif /* TYPEQUIV_H_ */
