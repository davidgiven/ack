#
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Header$ */

 mes 2,_EM_WSIZE,_EM_PSIZE
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
