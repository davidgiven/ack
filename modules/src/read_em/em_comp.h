/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/** @file
 *  A module to read EM assembly code in either
 *  human readable format or in compact format.
 *  It only permits to have one file open at a time,
 *  because it contains global variables.
 */

/* $Id$ */
#ifndef __EMCOMP_INCLUDED__
#define __EMCOMP_INCLUDED__

#include <ansi.h>
#include "em_arith.h"
#include "em_label.h"

/** Represents instruction arguments. */
struct e_arg {
	/** Type of this argument. The different argument
	 *  types are defined in `em_ptyp.h` */
	int	ema_argtype;
	union e_simple_arg {
		arith	emu_cst;	/**< a constant */
		label	emu_dlb;	/**< a numeric data label */
		label	emu_ilb;	/**< an instruction label */
		char	*emu_dnam;	/**< a data label */
		char	*emu_pnam;	/**< a procedure name */
		char	*emu_string;	/**< a string (fcon,icon,ucon,scon) */
	}	ema_arg; /**< The actual argument, a union representing the args. */
	arith	ema_szoroff;
};
#define ema_cst 	ema_arg.emu_cst
#define ema_dlb 	ema_arg.emu_dlb
#define ema_ilb 	ema_arg.emu_ilb
#define ema_dnam	ema_arg.emu_dnam
#define ema_pnam	ema_arg.emu_pnam
#define ema_nlocals	ema_szoroff
#define ema_string	ema_arg.emu_string

/** Represents an instruction and its arguments. */
struct e_instr {
	int     	em_type;	/**< Type of this instruction
	, one of the EM_XXXX constants. */
#define EM_MNEM 	256		/**< A machine instruction type */
#define EM_PSEU 	257		/**< A pseudo instruction type */
#define EM_STARTMES	258		/**< Start of a MES pseudo instruction */
#define EM_MESARG	259		/**< A MES argument type */
#define EM_ENDMES	260		/**< End of a MES  type */
#define EM_DEFILB	261		/**< An instruction label definition */
#define EM_DEFDLB	262		/**< A numeric data label definition */
#define EM_DEFDNAM	263		/**< A non-numeric data label definition */
#define EM_ERROR	264		/**< Recoverable error */
#define EM_FATAL	265		/**< Unrecoverable error */
#define EM_EOF  	266		/**< End of file */
	int     	em_opcode;  /**< opcode of the instruction. */
	struct e_arg	em_arg; /**< argument of the instruction. */
};

/** Initializes library to read from the specified file `filename`. If
 *  `filename` is a NULL pointer, reading is done from standard input.
 *  Returns 1 on success or 0 on failure with an error message in `EM_error`.
 */
int EM_open(char *filename);
/** Closes the library by freeing the resources that were previously opened
 *  using `EM_open()`.
 */
void EM_close(void);
/** Reads the next EM instruction, and returns it in the structure pointed to by
 *  `instr`. Returns 0 in case of error.
 */
int EM_getinstr(struct e_instr *instr);
int EM_mkcalls(struct e_instr *instr);

extern arith EM_holsize;
/** The size of the BSS block in bytes. */
#define EM_bsssize EM_holsize
/** 1 if the value should be initialized to the specified value,
 *  otherwise 0 */
extern int EM_holinit;
#define EM_bssinit EM_holinit

#define em_ilb  	em_arg.ema_ilb
#define em_dlb  	em_arg.ema_dlb
#define em_dnam 	em_arg.ema_dnam
#define em_argtype	em_arg.ema_argtype

#define em_cst  	em_arg.ema_cst
#define em_pnam 	em_arg.ema_pnam
#define em_nlocals 	em_arg.ema_nlocals
#define em_string	em_arg.ema_string
#define em_off  	em_arg.ema_szoroff
#define em_size 	em_arg.ema_szoroff

/* Arguments for EXC pseudo: */
#define em_exc1 	em_arg.ema_cst
#define em_exc2 	em_arg.ema_szoroff

extern char
	*EM_error, *EM_filename;

/** Line number of the last line read by `EM_getinstr()`. */
extern unsigned int EM_lineno;
/** Word size in bytes for this EM assembly file. The
 *  value is valid only after the first instruction
 *  has been read.
 */
extern int EM_wordsize;
/** Pointer size in bytes for this EM assembly file. The
 *  value is valid only after the first instruction
 *  has been read.
 */
extern int EM_pointersize;
#endif /* __EMCOMP_INCLUDED__ */
