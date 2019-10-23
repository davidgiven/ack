/*
	Main loop
*/

/* $Id$ */

#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include	<setjmp.h>

#include	<em_abs.h>
#include	"e.out.h"
#include	"logging.h"
#include	"nofloat.h"
#include	"global.h"
#include	"log.h"
#include	"io.h"
#include	"trap.h"
#include	"warn.h"
#include	"text.h"
#include	"read.h"
#include	"opcode.h"
#include	"m_sigtrp.h"
#include	"rsb.h"
#include	"whatever.h"

char mess_file[64] = "int.mess";	/* name of message file */

jmp_buf trapbuf;
char *prog_name;
int running;				/* set if EM machine is running */

size maxstack;				/* if set, max stack size */
size maxheap;				/* if set, max heap size */

#ifdef	LOGGING
extern long inr;			/* from log.c */
#endif	/* LOGGING */

PRIVATE char *dflt_av[] = {"e.out", 0};	/* default arguments */

/** Check dynamically that the interpreter can run on the target machine. */
static void check_requirements(char *name)
{
	
	/* Verify that shift right supported signed shifts. According to ISO C90,
	 * this is not mandatory, we should not support it here!
	 */
	int shrv = -4;
	if ((shrv >> 1) != -2)
	{
		fprintf(stderr,
			"%s compiled with compiler that does not support signed right shifts. Aborted.",
			name);
		exit(1);
	}
}

int main(int argc, char *argv[])
{
	register int i;
	register int nosetjmp = 1;
	int must_disassemble = 0;
	int must_tally = 0;


	prog_name = argv[0];

	check_requirements(prog_name);


	/* Initialize the EM machine */
	PreIgnMask = 0;
	FRALimit = FRALIMIT;
	
	for (i = 1; i < argc; i++) {
		if (*(argv[i]) == '-') {
			switch (*(argv[i] + 1)) {
			case 'd':	/* disassembly */
				must_disassemble = 1;
				break;
			case 'h':	/* limit heap size */
				maxheap = atol(argv[i] + 2);
				break;
			case 'I':	/* IgnMask pre-setting */
				if (atoi(argv[i] + 2) < 16)
					PreIgnMask = BIT(atoi(argv[i] + 2));
				break;
			case 'm':	/* messagefile name override */
				strcpy(mess_file, argv[i] + 2);
				break;
			case 'r':	/* FRALimit override */
				FRALimit = atoi(argv[i] + 2);
				break;
			case 's':	/* limit stack size */
				maxstack = atol(argv[i] + 2);
				break;
			case 't':	/* switch on tallying */
				must_tally= 1;
				break;
			case 'W':	/* disable warning */
				set_wmask(atoi(argv[i] + 2));
				break;
			default:
				fprintf(stderr,
					"%s: bad option: %s\n",
					prog_name,
					argv[i]
				);
				exit(1);
			}
		}
#ifdef	LOGGING
		else if (logarg(argv[i])) {
			/* interesting for the logging machine */
		}
#endif	/* LOGGING */
		else break;
	}

#ifdef	LOGGING
	/* Initialize the logging machine */
	init_log();
#endif	/* LOGGING */

	if (argc > i)
		init(argc - i, argv + i);
	else
		init(1, dflt_av);

	/* Text dump only? */
	if (must_disassemble) {
		message(
		    "text segment disassembly produced; program was not run");
		disassemble();
		close_down(0);
	}

	/* Analyse FLAGS word */
	if (FLAGS&FB_TEST)
		must_test = 1;

	if ((FLAGS&FB_PROFILE) || (FLAGS&FB_FLOW) || (FLAGS&FB_COUNT))
		must_tally = 1;

#ifdef	NOFLOAT
	if (FLAGS&FB_REALS)
		warning(WFLUSED);
#endif	/* NOFLOAT */

	if (FLAGS&FB_EXTRA)
		warning(WEXTRIGN);

	/* Call first procedure */
	running = 1;			/* start the machine */
	OnTrap = TR_HALT;		/* default trap handling */
	call(ENTRY, RSB_STP);

	/* Run the machine */
	while (running) {
#ifdef	LOGGING
		inr++;
		if (must_log && inr >= log_start) {
			/* log this instruction */
			logging = 1;
		}
#endif	/* LOGGING */

		LOG(("@x9 PC = %lu OPCODE = %lu", PC,
			btol(text_loc(PC)) < SECONDARY ?
				btol(text_loc(PC)) :
				btol(text_loc(PC)) + btol(text_loc(PC+1))
		));

		newPC(PC);		/* just check for validity */
		do_instr(nextPCbyte());	/* here it happens */

		if (must_tally) {
			tally();
		}

		if (signalled) {
			/* a signal has come in during this instruction */
			LOG(("@t1 signal %d caught by EM machine", signalled));
			trap_signal();
		}

		if (nosetjmp) {
			/* entry point after a trap occurred */
			setjmp(trapbuf);
			nosetjmp = 0;
		}

#ifdef	LOGGING
		log_eoi();
#endif	/* LOGGING */
	}
	
	if (must_tally) {
		out_tally();
	}
	
	if (ES_def == DEFINED) {
		message("program exits with status %ld", ES);
		close_down((int) ES);
	}
	else {
		message("program exits with undefined status");
		close_down(0);
	}
	/*NOTREACHED*/
}

