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
typedef struct idf t_idf;
typedef struct scope t_scope;
typedef struct node t_node;
typedef struct def t_def;
typedef struct scopelist t_scopelist;

t_def *lookup(register t_idf *id, t_scope *scope, int import, int flags);
t_def *lookfor(register t_node *id, register t_scopelist *vis, int message, int flags);

#endif /* LOOKUP_H_ */
