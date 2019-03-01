/*  Copyright (c) 2019 ACK Project.
 *  See the copyright notice in the ACK home directory,
 *  in the file "Copyright".
 *
 */
#ifndef __EVAL_H_INCLUDED__
#define __EVAL_H_INCLUDED__

#include "symbols.h"


/*--------------------------- Utilities -----------------------*/

/** From an internal type definition return the size in bytes of the data.
 *  If not a known data type return an integer size. */
extern int typesize(int ltype);
/** From an internal type definition return the size in bytes of the data. If not a
 *  known data type return a 0 size. */
extern int typestring(int type);


/*-------------------------- Code generation ---------------------*/

/** Emit code to convert from the old internal type to the new internal type definition.
 *  Generates an error if the type conversion is not allowed. */
extern void conversion(int oldtype,int newtype);
/** Emit code to store a value of the specified type from the top of stack
 *  into the specified variable pointed to by label.
 */
extern void storevar(int lab,int type);

/** Emit code to load a simple variable value on the stack. On input the address of the variable
 *  is on the stack.  */
extern void loadvar(int type);

extern void extraconvert(int oldtype,int newtype,int topstack);

extern void loadstr(int value);
extern int endarrayload(void);
extern void loadarray(int type);
extern void save_address(void);
extern void assign(int type,int lt);
extern int boolop(int ltype,int rtype,int operator);
extern int relop(int ltype,int rtype,int operator);
extern int plusmin(int ltype,int rtype,int operator);
extern int muldiv(int ltype,int rtype,int operator);
extern int negate(int type);
extern int power(int ltype,int rtype);
extern int loadint(int value);
extern int loaddbl(char* value);
extern int loadaddr(Symbol *s);
extern void newarrayload(Symbol *s);

#endif /* __EVAL_H_INCLUDED */
