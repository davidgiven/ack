#
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Id$ */

#include <ack/config.h>

 mes 2,_EM_WSIZE,_EM_PSIZE

#if ACKCONF_WANT_FLOAT
 exp $frexp
 pro $frexp,0
 lal 0
 loi _EM_DSIZE
 fef _EM_DSIZE
 lal _EM_DSIZE
 loi _EM_PSIZE
 sti _EM_WSIZE
 ret _EM_DSIZE
 end
#endif

