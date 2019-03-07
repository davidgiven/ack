/*  Copyright (c) 2019 ACK Project.
 *  See the copyright notice in the ACK home directory,
 *  in the file "Copyright".
 *
 */
#ifndef __GENCODE_H_INCLUDED__
#define __GENCODE_H_INCLUDED__

#include "symbols.h"


/*--------------------------- Utilities -----------------------*/

/** Return a global value identifier used for code generation */
extern int genlabel(void);


/*-------------------------- Code generation ---------------------*/

/** Emit a label definition and return the label identifier generated. */
extern int genemlabel(void);


void newemblock(int nr);
void newblock(int nr);
void datastmt(void);
void datatable(void);
/* ERROR and exception handling */
void exceptstmt(int lab);
void errorstmt(int exprtype);
/* BASIC IO */
void openstmt(int recsize);
void printstmt(int exprtype);
void zone(int i);
void writestmt(int exprtype, int comma);
void restore(int lab);
void prompt(int qst);
void linestmt(int type);
void readelm(int type);
/* Swap exchanges the variable values */
void swapstmt(int ltype,int rtype);
void setchannel(int val);
int ifstmt(int type);
int thenpart(int elselab);
void elsepart(int lab);
void forinit(Symbol *s);
void forexpr(int type);
void forlimit(int type);
void forstep(int type);
void nextstmt(Symbol *s);
void pokestmt(int type1,int type2);
void whilestart(void);
void whiletst(int exprtype);
void wend(void);
/* generate code for the final version */
void prologcode(void);
void prolog2(void);
void gendata(void);
void epilogcode(void);
void setchannel(int val);
void gendata(void);


#endif /* __GENCODE_H_INCLUDED__ */
