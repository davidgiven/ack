/*
 * error.h
 *
 *  Created on: 2018-11-17
 *      Author: carl
 */


#ifndef __ERROR_H_INCLUDED__
#define __ERROR_H_INCLUDED__

void stop(void);
void fatal(char *format, ...);
void warning(char *format, ...);
void error(char *format, ...);
int do_debug(char *format, ...);
int do_verbose(char *format, ...);


#endif /* __ERROR_H_INCLUDED__ */
