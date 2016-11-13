/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 */

#include "ack.h"

#ifndef NORCSID
static char rcs_id[] = "$Id$" ;
#endif

/*      The processing of string valued variables,
	this is an almost self contained module.

	Five externally visible routines:

	setsvar(name,result)
		Associate the name with the result.

		name    a string pointer
		result  a string pointer

	setpvar(name,routine)
		Associate the name with the routine.

		name    a string pointer
		routine a routine id

	   The parameters name and result are supposed to be pointing to
	   non-volatile string storage used only for this call.

	char *getvar(name)
		returns the pointer to a string associated with name,
		the pointer is produced by returning result or the
		value returned by calling the routine.

		name    a string pointer

	Other routines called

	fatal(args*)    When something goes wrong
	getcore(size)   Core allocation

*/

struct vars {
	char                            *v_name;
	enum { routine, string }        v_type;

	union {
		char    *v_string;
		char    *(*v_routine)();
	}                               v_value ;
	struct vars                     *v_next ;
};

static struct vars *v_first ;

static struct vars *newvar(char *name) {
	register struct vars *new ;

	for ( new=v_first ; new ; new= new->v_next ) {
		if ( strcmp(name,new->v_name)==0 ) {
			throws(name) ;
			if ( new->v_type== string ) {
				throws(new->v_value.v_string) ;
			}
			return new ;
		}
	}
	new= (struct vars *)getcore(sizeof (struct vars));
	new->v_name= name ;
	new->v_next= v_first ;
	v_first= new ;
	return new ;
}

void setsvar(char *name, char *str) {
	register struct vars *new ;

	new= newvar(name);
#ifdef DEBUG
	if ( debug>=2 ) vprint("%s=%s\n", name, str) ;
#endif
	new->v_type= string;
	new->v_value.v_string= str;
}

void setpvar(char *name, char *(*rout)(void)) {
	register struct vars *new ;

	new= newvar(name);
#ifdef DEBUG
	if ( debug>=2 ) vprint("%s= (*%o)()\n",name,rout) ;
#endif
	new->v_type= routine;
	new->v_value.v_routine= rout;
}

char *getvar(const char *name) {
	register struct vars *scan ;

	for ( scan=v_first ; scan ; scan= scan->v_next ) {
		if ( strcmp(name,scan->v_name)==0 ) {
			switch ( scan->v_type ) {
			case string:
				return scan->v_value.v_string ;
			case routine:
				return (*scan->v_value.v_routine)() ;
			}
		}
	}
	return (char *)0 ;
}
