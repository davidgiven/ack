/*  Copyright (c) 2019 ACK Project.
 *  See the copyright notice in the ACK home directory,
 *  in the file "Copyright".
 *
 */
#ifndef ERROR_H_
#define ERROR_H_

/* Forward struct declarations */
struct node;

#ifdef DEBUG
void debug(char *fmt, ...);
#endif /* DEBUG */

void error(char *fmt, ...);
void node_error(struct node *node, char *fmt, ...);
void warning(char *fmt, ...);
void node_warning(struct node *node, char *fmt, ...);
void lexerror(char *fmt, ...);
void lexwarning(char *fmt, ...);
void fatal(char *fmt, ...);
void crash(char *fmt, ...);

#endif /* ERROR_H_ */
