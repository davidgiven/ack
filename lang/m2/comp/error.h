/*  Copyright (c) 2019 ACK Project.
 *  See the copyright notice in the ACK home directory, 
 *  in the file "Copyright".
 *
 *  Created on: 2019-02-26
 *  
 */
#ifndef ERROR_H_
#define ERROR_H_

struct node;

#if __STDC__
#ifdef DEBUG
/*VARARGS*/
void debug(char *fmt, ...);
#endif /* DEBUG */

/*VARARGS*/
void error(char *fmt, ...);
/*VARARGS*/
void node_error(struct node *node, char *fmt, ...);
/*VARARGS*/
void warning(int class, char *fmt, ...);
/*VARARGS*/
void node_warning(struct node *node, int class, char *fmt, ...);
/*VARARGS*/
void lexerror(char *fmt, ...);
/*VARARGS*/
void lexwarning(int class, char *fmt, ...);
/*VARARGS*/
void fatal(char *fmt, ...);
/*VARARGS*/
void crash(char *fmt, ...);
#else

#ifdef DEBUG
/*VARARGS*/
void debug(va_alist);
#endif /* DEBUG */

/*VARARGS*/
void error(va_alist);
/*VARARGS*/
void node_error(va_alist);
/*VARARGS*/
void warning(va_alist);
/*VARARGS*/
void node_warning(va_alist);
/*VARARGS*/
void lexerror(va_alist);
/*VARARGS*/
void lexwarning(va_alist);
/*VARARGS*/
void fatal(va_alist);
/*VARARGS*/
void crash(va_alist);
#endif

#endif /* ERROR_H_ */
