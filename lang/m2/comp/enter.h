/*  Copyright (c) 2019 ACK Project.
 *  See the copyright notice in the ACK home directory, 
 *  in the file "Copyright".
 *
 *  Created on: 2019-02-27
 *  
 */
#ifndef ENTER_H_
#define ENTER_H_

/* Forward declarations. */
struct type;
struct node;
struct scope;
struct paramlist;
struct def;


struct def *Enter(char *name, int kind, struct type  *type, int pnam);
struct def *EnterType(char *name, struct type  *type);
void EnterEnumList(struct node *Idlist, register struct type  *type);
void EnterFieldList(struct node *Idlist, register struct type  *type, struct scope *scope,
	arith *addr);
/*	Enter a list of identifiers representing variables into the
	name list. "type" represents the type of the variables.
	"local" is set if the variables are declared local to a
	procedure.
*/
void EnterVarList(struct node *Idlist, struct type  *type, int local);
/*	Create (part of) a parameterlist of a procedure.
	"ids" indicates the list of identifiers, "tp" their type, and
	"VARp" indicates D_VARPAR or D_VALPAR.
*/
void EnterParamList(struct paramlist **ppr, struct node *Idlist, struct type  *type,
	int VARp, arith *off);
/*	From the current scope, the list of identifiers "ids" is
	exported. Note this fact. If the export is not qualified, make
	all the "ids" visible in the enclosing scope by defining them
	in this scope as "imported".
*/
void EnterExportList(struct node *Idlist, int qualified);
void CheckForImports(struct def *df);
void EnterFromImportList(struct node *idlist, struct def *FromDef, struct node *FromId);
void EnterImportList(struct node *idlist, int local, struct scope *sc);

#endif /* ENTER_H_ */
