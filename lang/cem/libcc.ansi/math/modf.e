#
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Header$ */

 mes 2,EM_WSIZE,EM_PSIZE
#ifndef NOFLOAT
 exp $modf
 pro $modf,0
 lal 0
 loi EM_DSIZE
 loc 1
 loc EM_WSIZE
 loc EM_DSIZE
 cif
 fif EM_DSIZE
 lal EM_DSIZE
 loi EM_PSIZE
 sti EM_DSIZE
 ret EM_DSIZE
 end
#endif
