/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/*
 * $Id$
 */
/*
 * Values depend on the machine on which this program should run.
 * Now for Vax 11/750.
 */
#ifndef __MACH_H_INCLUDED__
#define __MACH_H_INCLUDED__


#include "local.h"

#define K 1024L

#if BIGMACHINE
#define INCRSIZE 8192
	mems[ALLOEMIT + 0].mem_left = 64 * K;
	mems[ALLOEMIT + 1].mem_left = 4 * K;
	mems[ALLOEMIT + 2].mem_left = 16 * K;
	mems[ALLORELO].mem_left = 24 * K;
	mems[ALLOLOCL].mem_left = 16 * K;
	mems[ALLOGLOB].mem_left = 16 * K;
	mems[ALLOLCHR].mem_left = 16 * K;
	mems[ALLOGCHR].mem_left = 16 * K;
#ifdef SYMDBUG
	mems[ALLODBUG].mem_left = 32 * K;
#endif /* SYMDBUG */
	mems[ALLOSYMB].mem_left = 8 * K;
	mems[ALLOARCH].mem_left = 4 * K;
	mems[ALLOMODL].mem_left = 64 * K;
	mems[ALLORANL].mem_left = 8 * K;
#else
#define INCRSIZE 256
	mems[ALLOEMIT + 0].mem_left = 8 * K;
	mems[ALLOEMIT + 1].mem_left = 2 * K;
	mems[ALLOEMIT + 2].mem_left = 4 * K;
	mems[ALLORELO].mem_left = 2 * K;
	mems[ALLOLOCL].mem_left = 2 * K;
	mems[ALLOGLOB].mem_left = 2 * K;
	mems[ALLOLCHR].mem_left = 2 * K;
	mems[ALLOGCHR].mem_left = 2 * K;
#ifdef SYMDBUG
	mems[ALLODBUG].mem_left = 2 * K;
#endif /* SYMDBUG */
	mems[ALLOSYMB].mem_left = 2 * K;
	mems[ALLOARCH].mem_left = 1 * K;
	mems[ALLOMODL].mem_left = 12 * K;
	mems[ALLORANL].mem_left = 4 * K;
#endif

#endif /* __MACH_H_INCLUDED__ */
