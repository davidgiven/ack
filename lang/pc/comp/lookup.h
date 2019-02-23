/*  Copyright (c) 2019 ACK Project.
 *  See the copyright notice in the ACK home directory,
 *  in the file "Copyright".
 *
 */
#ifndef LOOKUP_H_
#define LOOKUP_H_

struct def;
struct idf;
struct scope;
struct node;

void remove_def(register struct def *df);

/*	Look up a definition of an identifier in scope "scope".
	Make the "def" list self-organizing.
	Return a pointer to its "def" structure if it exists,
	otherwise return NULL.
*/
struct def *lookup(register struct idf *id, struct scope *scope, long inuse);

/*	Look for an identifier in the visibility range started by "vis".
	If it is not defined create a dummy definition and
	if give_error is set, give an error message.
*/
struct def *lookfor(register struct node *id, struct scopelist *vis, int give_error);

#endif /* LOOKUP_H_ */
