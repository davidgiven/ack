/*  Copyright (c) 2019 ACK Project.
 *  See the copyright notice in the ACK home directory, 
 *  in the file "Copyright".
 *
 *  Created on: 2019-02-06
 *  
 */
#ifndef ERROR_H_
#define ERROR_H_

struct expr;

#if __STDC__
/*VARARGS*/
void error(char *fmt, ...);
/*VARARGS*/
void expr_error(struct expr *expr, char *fmt, ...);
/*VARARGS*/
void lexstrict(char *fmt, ...);
/*VARARGS*/
void strict(char *fmt, ...);
/*VARARGS*/
void expr_strict(struct expr *expr, char *fmt, ...);

#ifdef DEBUG
/*VARARGS*/
void debug(char *fmt, ...);
#endif /* DEBUG */

/*VARARGS*/
void warning(char *fmt, ...);
/*VARARGS*/
void expr_warning(struct expr *expr, char *fmt, ...);
#ifdef	LINT
/*VARARGS*/
void def_warning(struct def *def, char *fmt, ...);
/*VARARGS*/
void hwarning(char *fmt, ...);
/*VARARGS*/
void awarning(char *fmt, ...);
#endif	/* LINT */

/*VARARGS*/
void lexerror(char *fmt, ...);
/*VARARGS*/
void lexwarning(char *fmt, ...);
/*VARARGS*/
void crash(char *fmt, ...);
/*VARARGS*/
void fatal(char *fmt, ...);

#else
/*VARARGS*/
void error(va_alist);				/* fmt, args */
/*VARARGS*/
void expr_error(va_alist);			/* expr, fmt, args */
/*VARARGS*/
void lexstrict(va_alist);
/*VARARGS*/
void strict(va_alist);
/*VARARGS*/
void expr_strict(va_alist);			/* expr, fmt, args */
#ifdef DEBUG
/*VARARGS*/
void debug(va_alist);
#endif /* DEBUG */

/*VARARGS*/
void warning(va_alist);
/*VARARGS*/
void expr_warning(va_alist);			/* expr, fmt, args */
#ifdef	LINT

/*VARARGS*/
void def_warning(va_alist);			/* def, fmt, args */
/*VARARGS*/
void hwarning(va_alist);			/* fmt, args */
/*VARARGS*/
void awarning(va_alist);			/* fmt, args */
#endif	/* LINT */

/*VARARGS*/
void lexerror(va_alist);			/* fmt, args */
/*VARARGS*/
void lexwarning(va_alist);			/* fmt, args */
/*VARARGS*/
void crash(va_alist);				/* fmt, args */
/*VARARGS*/
void fatal(va_alist);				/* fmt, args */
#endif



#endif /* ERROR_H_ */
