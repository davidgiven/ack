/*
	Core dumping routines
*/

/* $Header$ */

#include	"logging.h"
#include	"global.h"
#include	"shadow.h"
#include	"fra.h"

#include	<stdio.h>

core_dump()
{
	FILE *core_file;
	
	core_file = fopen("int.core", "w");
	if (!core_file) {
		/* no point in giving a fatal error again! */
		return;
	}

/******** EM Machine capacity parameters ********/

	fprintf(core_file, "wsize=%ld\n", wsize);
	fprintf(core_file, "psize=%ld\n", psize);

/******** EM program parameters ********/

	fprintf(core_file, "ML=%lu\n", ML);
	fprintf(core_file, "HB=%lu\n", HB);
	fprintf(core_file, "DB=%lu\n", DB);
	fprintf(core_file, "NProc=%ld\n", NProc);

/******** EM machine registers ********/

	fprintf(core_file, "PI=%ld\n", PI);
	fprintf(core_file, "PC=%lu\n", PC);

	fprintf(core_file, "HP=%lu\n", HP);
	fprintf(core_file, "SP=%lu\n", SP);
	fprintf(core_file, "LB=%lu\n", LB);
	fprintf(core_file, "AB=%lu\n", AB);

	fprintf(core_file, "ES=%ld\n", ES);
	fprintf(core_file, "ES_def=%d\n", ES_def);

	fprintf(core_file, "OnTrap=%d\n", OnTrap);
	fprintf(core_file, "IgnMask=%ld\n", IgnMask);
	fprintf(core_file, "TrapPI=%ld\n", TrapPI);

	fprintf(core_file, "FRASize=%ld\n", FRASize);
	fprintf(core_file, "FRA_def=%d\n", FRA_def);

	fprintf(core_file, "HL=%lu\n", HL);
	fprintf(core_file, "SL=%lu\n", SL);

/******** The EM machine memory ********/

	fwrite(text, 1, (int)(DB), core_file);
	fwrite(data, 1, (int)(HL), core_file);
	fwrite(stack, 1, (int)(ML+1-SL), core_file);
	fwrite(FRA, 1, (int)(FRALimit), core_file);

#ifdef	LOGGING
	fwrite(FRA_sh, 1, (int)(FRALimit), core_file);
	fwrite(data_sh, 1, (int)(HL), core_file);
	fwrite(stack_sh, 1, (int)(ML+1-SL), core_file);
#endif	/* LOGGING */
	
	fclose(core_file);
	core_file = 0;
}

