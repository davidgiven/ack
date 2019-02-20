/*  Copyright (c) 2019 ACK Project.
 *  See the copyright notice in the ACK home directory, 
 *  in the file "Copyright".
 *
 *  Created on: 2019-02-09
 *  
 */
#ifndef ERROR_H_
#define ERROR_H_

#if __STDC__
/*VARARGS*/
void error(char *fmt, ...);
/*VARARGS*/
void warning(char *fmt, ...);
/*VARARGS*/
void strict(char *fmt, ...);
/*VARARGS*/
void crash(char *fmt, ...);
/*VARARGS*/
void fatal(char *fmt, ...);
#else
/*VARARGS*/
void error(va_alist);
	va_dcl

/*VARARGS*/
void warning(va_alist);
	va_dcl

/*VARARGS*/
void strict(va_alist);
	va_dcl
/*VARARGS*/
void crash(va_alist);
	va_dcl
/*VARARGS*/
void fatal(va_alist);
	va_dcl
#endif

#endif /* ERROR_H_ */
