/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Id$ */

#ifndef __DEBUG_H_INCLUDED__
#define __DEBUG_H_INCLUDED__

#include "error.h"

#ifdef NDEBUG

#define debug(s, a1, a2, a3, a4)

#else
extern int DEB;

#define debug(s, a1, a2, a3, a4)	(DEB && do_debug(s, a1, a2, a3, a4))

#endif

extern int Verbose;
#define verbose(s, a1, a2, a3, a4)	(Verbose && do_verbose(s, a1, a2, a3, a4))

#endif /* __DEBUG_H_INCLUDED__ */
