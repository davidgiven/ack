/* $Header$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/*	This file defines size_type if _EM_WSIZE and _EM_PSIZE are defined.
*/

#if defined(_EM_WSIZE) && defined(_EM_PSIZE)
#if _EM_WSIZE == _EM_PSIZE
#define size_type	unsigned int
#else
#define size_type	long
#endif
#endif
