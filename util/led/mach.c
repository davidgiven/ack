/*
 * $Header$
 */
/*
 * Values depend on the machine on which this program should run.
 * Now for Vax 11/750.
 */

#define K 1024

	mems[ALLOEMIT + 0].mem_left = 64 * K;
	mems[ALLOEMIT + 1].mem_left = 64 * K;
	mems[ALLORELO].mem_left = 64 * K;
	mems[ALLOLOCL].mem_left = 64 * K;
	mems[ALLOGLOB].mem_left = 64 * K;
	mems[ALLOLCHR].mem_left = 64 * K;
	mems[ALLOGCHR].mem_left = 64 * K;
#ifdef SYMDBUG
	mems[ALLODBUG].mem_left = 64 * K;
#endif SYMDBUG
	mems[ALLOSYMB].mem_left = 4 * K;
	mems[ALLOARCH].mem_left = 1 * K;
	mems[ALLOMODL].mem_left = 3 * 64 * K;
	mems[ALLORANL].mem_left = 4 * K;
