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
typedef struct type t_type;
typedef struct def  t_def;
typedef struct node t_node;
typedef struct scope t_scope;
typedef struct paramlist t_param;


t_def *Enter(char *name, int kind, t_type *type, int pnam);
t_def *EnterType(char *name, t_type *type);
void EnterEnumList(t_node *Idlist, register t_type *type);
void EnterFieldList(t_node *Idlist, register t_type *type, t_scope *scope,
	arith *addr);
/*	Enter a list of identifiers representing variables into the
	name list. "type" represents the type of the variables.
	"local" is set if the variables are declared local to a
	procedure.
*/
void EnterVarList(t_node *Idlist, t_type *type, int local);
/*	Create (part of) a parameterlist of a procedure.
	"ids" indicates the list of identifiers, "tp" their type, and
	"VARp" indicates D_VARPAR or D_VALPAR.
*/
void EnterParamList(t_param **ppr, t_node *Idlist, t_type *type,
	int VARp, arith *off);
/*	From the current scope, the list of identifiers "ids" is
	exported. Note this fact. If the export is not qualified, make
	all the "ids" visible in the enclosing scope by defining them
	in this scope as "imported".
*/
void EnterExportList(t_node *Idlist, int qualified);
void CheckForImports(t_def *df);
void EnterFromImportList(t_node *idlist, t_def *FromDef, t_node *FromId);
void EnterImportList(t_node *idlist, int local, t_scope *sc);

#endif /* ENTER_H_ */
