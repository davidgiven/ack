#
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Header$ */

 mes 2,EM_WSIZE,EM_PSIZE
#ifndef NOFLOAT
 exp $frexp
 pro $frexp,0
 lal 0
 loi EM_DSIZE
 fef EM_DSIZE
 lal EM_DSIZE
 loi EM_PSIZE
 sti EM_WSIZE
 ret EM_DSIZE
 end
#endif
