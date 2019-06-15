/*  Copyright (c) 2019 ACK Project.
 *  See the copyright notice in the ACK home directory, 
 *  in the file "Copyright".
 *
 *  Created on: 2019-02-27
 *  
 */
#ifndef LOOKUP_H_
#define LOOKUP_H_

/* Forward declarations. */
struct idf;
struct scope;
struct node;
struct scopelist;
struct def;

struct def *lookup(register struct idf *id, struct scope *scope, int import, int flags);
struct def *lookfor(register struct node *id, register struct scopelist *vis, int message, int flags);

#endif /* LOOKUP_H_ */
