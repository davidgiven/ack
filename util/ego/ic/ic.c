/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* I N T E R M E D I A T E   C O D E
 *
 * I C . C
 */

#include <stdlib.h>
#include <stdio.h>
#include <em_spec.h>
#include <em_pseu.h>
#include <em_flag.h>
#include <em_mes.h>
#include "../share/types.h"
#include "../share/debug.h"
#include "../share/def.h"
#include "../share/map.h"
#include "ic.h"
#include "ic_lookup.h"
#include "ic_aux.h"
#include "ic_io.h"
#include "ic_lib.h"
#include "../share/alloc.h"
#include "../share/global.h"
#include "../share/files.h"
#include "../share/put.h"
#include "../share/aux.h"


/* Global variables */


dblock_p	db;
dblock_p	hol0_db;	/* dblock for ABS block */
char		*curhol;	/* name of hol block in current scope */
dblock_p	ldblock;	/* last dblock  */
proc_p		lproc;		/* last proc    */
short		tabval;		/* used by table1, table2 and table3 */
offset		tabval2;
char		string[IDL+1];
line_p		firstline;	/* first line of current procedure */
line_p		lastline;	/* last line read */
int		labelcount;	/* # labels in current procedure */
short		fragm_type = DUNKNOWN; /* fragm. type: DCON, DROM or DUNKNOWN */
short		fragm_nr = 0;	/* fragment number */
obj_id		lastoid = 0;
proc_id		lastpid = 0;
dblock_id	lastdid = 0;
lab_id		lastlid = 0;

offset 		mespar = UNKNOWN_SIZE;
		/* argumument of ps_par message of current procedure */


extern		process_lines();
extern int	readline();
extern line_p	readoperand();
extern line_p	inpseudo();


main(argc,argv)
	int argc;
	char *argv[];
{
	/* The input files must be legal EM Compact
	 * Assembly Language files, as produced by the EM Peephole
	 * Optimizer.
	 * Their file names are passed as arguments.
	 * The output consists of the files:
	 *  - lfile: the EM code in Intermediate Code format
	 *  - dfile: the data block table file
	 *  - pfile: the proc table file
	 *  - pdump: the names of all procedures
	 *  - ddump: the names of all data blocks
	 */

	FILE  *lfile, *dfile, *pfile, *pdump, *ddump;

	lfile = openfile(lname2,"w");
	pdump = openfile(argv[1],"w");
	ddump = openfile(argv[2],"w");
	hol0_db = block_of_lab((char *) 0);
	while (next_file(argc,argv) != NULL) {
		/* Read all EM input files, process the code
		 * and concatenate all output.
		 */
		curhol = (char *) 0;
		process_lines(lfile);
		dump_procnames(prochash,NPROCHASH,pdump);
		dump_dblocknames(symhash,NSYMHASH,ddump);
		/* Save the names of all procedures that were
		 * first come accross in this file.
		 */
		cleanprocs(prochash,NPROCHASH,PF_EXTERNAL);
		cleandblocks(symhash,NSYMHASH,DF_EXTERNAL);
		/* Make all procedure names that were internal
		 * in this input file invisible.
		 */
	}
	fclose(lfile);
	fclose(pdump);
	fclose(ddump);


	/* remove the remainder of the hashing tables */
	cleanprocs(prochash,NPROCHASH,0);
	cleandblocks(symhash,NSYMHASH,0);
	/* Now write the datablock table and the proctable */
	dfile = openfile(dname2,"w");
	putdtable(fdblock, dfile);
	pfile = openfile(pname2,"w");
	putptable(fproc, pfile,FALSE);
	exit(0);
}



/* Value returned by readline */

#define NORMAL		0
#define WITH_OPERAND	1
#define EOFILE		2
#define PRO_INSTR	3
#define END_INSTR	4
#define DELETED_INSTR	5


STATIC add_end()
{
	/* Add an end-pseudo to the current instruction list */

	lastline->l_next = newline(OPNO);
	lastline = lastline->l_next;
	lastline->l_instr = ps_end;
}


process_lines(fout)
	FILE *fout;
{
	line_p lnp;
	short   instr;
	bool   eof;

	/* Read and process the code contained in the current file,
	 * on a per procedure basis.
	 * On the fly, fragments are formed. Recall that two
	 * successive CON pseudos are allocated consecutively
	 * in a single fragment, unless these CON pseudos are
	 * separated in the assembly language program by one
	 * of: ROM, BSS, HOL and END (and of course EndOfFile).
	 * The same is true for ROM pseudos.
	 * We keep track of a fragment type (DROM after a ROM
	 * pseudo, DCON after a CON and DUNKNOWN after a HOL,
	 * BSS, END or EndOfFile) and a fragment number (which
	 * is incremented every time we enter a new fragment).
	 * Every data block is assigned such a number
	 * when we come accross its defining occurrence.
	 */

	eof = FALSE;
	firstline = (line_p) 0;
	lastline = (line_p) 0;
	while (!eof) {
		linecount++;	/* for error messages */
		switch(readline(&instr, &lnp)) {
			/* read one line, see what kind it is */
			case WITH_OPERAND:
				/* instruction with operand, e.g. LOL 10 */
				lnp = readoperand(instr);
				lnp->l_instr = instr;
				/* Fall through! */
			case NORMAL:
				VL(lnp);
				if (lastline != (line_p) 0) {
					lastline->l_next = lnp;
				}
				lastline = lnp;
				break;
			case EOFILE:
				eof = TRUE;
				fragm_type = DUNKNOWN;
				if (firstline != (line_p) 0) {
					add_end();
					putlines(firstline,fout);
					firstline = (line_p) 0;
				}
				break;
			case PRO_INSTR:
				VL(lnp);
				labelcount = 0;
				if (firstline != lnp) {
					/* If PRO is not the first
					 * instruction:
					 */
					add_end();
					putlines(firstline,fout);
					firstline = lnp;
				}
				lastline = lnp;
				break;
			case END_INSTR:
				curproc->p_nrformals = mespar;
				mespar = UNKNOWN_SIZE;
				assert(lastline != (line_p) 0);
				lastline->l_next = lnp;
				putlines(firstline,fout);
				/* write and delete code */
				firstline = (line_p) 0;
				lastline = (line_p) 0;
				cleaninstrlabs();
				/* scope of instruction labels ends here,
				 * so forget about them.
				 */
				fragm_type = DUNKNOWN;
				break;
			case DELETED_INSTR:
				/* EXP, INA etc. are deleted */
				break;
			default:
				error("illegal readline");
		}
	}
}



int readline(instr_out, lnp_out)
	short  *instr_out;
	line_p *lnp_out;
{
	register line_p lnp;
	short n;

	/* Read one line. If it is a normal EM instruction without
	 * operand, we can allocate a line struct for it here.
	 * If so, return a pointer to it via lnp_out, else just
	 * return the instruction code via instr_out.
	 */

	VA((short *) instr_out);
	VA((short *) lnp_out);
	switch(table1()) {
		/* table1 sets string, tabval or tabval2 and
		 * returns an indication of what was read.
		 */
		case ATEOF:
			return EOFILE;
		case INST:
			*instr_out = tabval; /* instruction code */
			return WITH_OPERAND;
		case DLBX:
			/* data label defining occurrence, precedes
			 * a data block.
			 */
			db = block_of_lab(string);
			/* global variable, used by inpseudo */
			lnp = newline(OPSHORT);
			SHORT(lnp) = (short) db->d_id;
			lnp->l_instr = ps_sym;
			*lnp_out = lnp;
			if (firstline == (line_p) 0) {
				firstline = lnp;
				/* only a pseudo (e.g. PRO) or data label
				 * can be the first instruction.
				 */
			}
			return NORMAL;
		case ILBX:
			/* instruction label defining occurrence */
			labelcount++;
			lnp = newline(OPINSTRLAB);
			lnp->l_instr = op_lab;
			INSTRLAB(lnp) = instr_lab(tabval);
			*lnp_out = lnp;
			return NORMAL;
		case PSEU:
			n = tabval;
			lnp = inpseudo(n); /* read a pseudo */
			if (n == ps_hol) n = ps_bss;
			if (lnp == (line_p) 0)  return DELETED_INSTR;
			*lnp_out = lnp;
			lnp->l_instr = n;
			if (firstline == (line_p) 0) {
				firstline = lnp;
				/* only a pseudo (e.g. PRO) or data label
				 * can be the first instruction.
				 */
			}
			if (n == ps_end)  return END_INSTR;
			if (n == ps_pro)  return PRO_INSTR;
			return NORMAL;
	}
	/* NOTREACHED */
}


line_p readoperand(instr)
	short instr;
{
	/* Read the operand of the given instruction.
	 * Create a line struct and return a pointer to it.
	 */


	register line_p lnp;
	short flag;

	VI(instr);
	flag = em_flag[ instr - sp_fmnem] & EM_PAR;
	if (flag == PAR_NO) {
		return (newline(OPNO));
	}
	switch(table2()) {
		case sp_cend:
			return(newline(OPNO));
		case CSTX1:
			/* constant */
			/* If the instruction has the address
			 * of an external variable as argument,
			 * the constant must be regarded as an
			 * offset in the current hol block,
			 * so an object must be created.
			 * Similarly, the instruction may have
			 * an instruction label as argument.
			 */
			switch(flag) {
			   case PAR_G:
				lnp = newline(OPOBJECT);
				OBJ(lnp) =
				  object(curhol,(offset) tabval,
					 opr_size(instr));
				break;
			   case PAR_B:
				lnp = newline(OPINSTRLAB);
				INSTRLAB(lnp) = instr_lab(tabval);
				break;
			   default:
				lnp = newline(OPSHORT);
				SHORT(lnp) = tabval;
				break;
			}
			break;
#ifdef LONGOFF
		case CSTX2:
			/* double constant */
			if (flag == PAR_G) {
				lnp = newline(OPOBJECT);
				OBJ(lnp) =
				  object(curhol, tabval2,
					 opr_size(instr));
				break;
			}
			lnp = newline(OPOFFSET);
			OFFSET(lnp) = tabval2;
			break;
#endif
		case ILBX:
			/* applied occurrence instruction label */
			lnp = newline(OPINSTRLAB);
			INSTRLAB(lnp) = instr_lab(tabval);
			break;
		case DLBX:
			/* applied occurrence data label */
			lnp = newline(OPOBJECT);
			OBJ(lnp) = object(string, (offset) 0,
					opr_size(instr) );
			break;
		case VALX1:
			lnp = newline(OPOBJECT);
			OBJ(lnp) = object(string, (offset) tabval,
					opr_size(instr) );
			break;
#ifdef LONGOFF
		case VALX2:
			lnp = newline(OPOBJECT);
			OBJ(lnp) = object(string,tabval2,
					opr_size(instr) );
			break;
#endif
		case sp_pnam:
			lnp = newline(OPPROC);
			PROC(lnp) = proclookup(string,OCCURRING);
			VP(PROC(lnp));
			break;
		default:
			assert(FALSE);
	}
	return lnp;
}


static char *hol_label()
{
	static int holno;
	line_p lnp;
	extern char *lastname;

	/* Create a label for a hol pseudo, so that it can be converted
	 * into a bss. The label is appended to the list of instructions.
	 */

	sprintf(string, "_HH%d", ++holno);
	symlookup(string, OCCURRING);		/* to make it exa */
	db = block_of_lab(string);
	lnp = newline(OPSHORT);
	SHORT(lnp) = (short) db->d_id;
	lnp->l_instr = ps_sym;
	if (firstline == (line_p) 0) {
		firstline = lnp;
	}
	if (lastline != (line_p) 0) {
		lastline->l_next = lnp;
	}
	lastline = lnp;
	return lastname;
}


line_p inpseudo(n)
	short n;
{
	int m;
	line_p lnp;
	byte pseu;
	short nlast;

	/* Read the (remainder of) a pseudo instruction, the instruction
	 * code of which is n. The END pseudo may be deleted (return 0).
	 * The pseudos INA, EXA, INP and EXP (visibility pseudos) must
	 * also be deleted, although the effects they have on the
	 * visibility of global names and procedure names must first
	 * be recorded in the datablock or procedure table.
	 */


	switch(n) {
		case ps_hol:
			/* hol pseudos are carefully converted into bss
			 * pseudos, so that the IL phase will not be
			 * bothered by this. Also, references to the ABS
			 * block will still work when passed through EGO.
			 */
			
			if (lastline != (line_p) 0 && is_datalabel(lastline)) {
				extern char *lastname;

				curhol = lastname;
			}
			else {
				curhol = hol_label();
			}
			n = ps_bss;
			/* fall through */			
		case ps_bss:
		case ps_rom:
		case ps_con:
			if (lastline == (line_p) 0 || !is_datalabel(lastline)) {
				assert(lastline != (line_p) 0);
				nlast = INSTR(lastline);
				if (n == nlast &&
					(n == ps_rom || n == ps_con)) {
					/* Two successive roms/cons are
					 * combined into one data block
					 * if the second is not preceded by
					 * a data label.
					 */
					lnp = arglist(0);
					pseu = (byte) (n == ps_rom?DROM:DCON);
					combine(db,lastline,lnp,pseu);
					oldline(lnp);
					return (line_p) 0;
				} else {
				   	error("datablock without label");
				}
			}
			VD(db);
			m = (n == ps_bss ? 3 : 0);
			lnp = arglist(m);
			/* Read the arguments, 3 for hol or bss and a list
			 * of undetermined length for rom and con.
			 */
			dblockdef(db,n,lnp);
			/* Fill in d_pseudo, d_size and d_values fields of db */
			if (fragm_type != db->d_pseudo) {
				/* Keep track of fragment numbers,
				 * enter a new fragment.
				 */
				fragm_nr++;
				switch(db->d_pseudo) {
					case DCON:
					case DROM:
						fragm_type = db->d_pseudo;
						break;
					default:
						fragm_type = DUNKNOWN;
						break;
				}
			}
			db->d_fragmnr = fragm_nr;
			return lnp;
		case ps_ina:
			getsym(DEFINING);
			/* Read and lookup a symbol. As this must be
			 * the first occurrence of the symbol and we
			 * say it's a defining occurrence, getsym will
			 * automatically make it internal (according to
			 * the EM visibility rules).
			 * The result (a dblock pointer) is voided.
			 */
			return (line_p) 0;
		case ps_inp:
			getproc(DEFINING);  /* same idea */
			return (line_p) 0;
		case ps_exa:
			getsym(OCCURRING);
			return (line_p) 0;
		case ps_exp:
			getproc(OCCURRING);
			return (line_p) 0;
		case ps_pro:
			curproc = getproc(DEFINING);
			/* This is a real defining occurrence of a proc */
			curproc->p_localbytes = get_off();
			curproc->p_flags1 |= PF_BODYSEEN;
			/* Record the fact that we came accross
			 * the body of this procedure.
			 */
			lnp = newline(OPPROC);
			PROC(lnp) = curproc;
			lnp->l_instr = (byte) ps_pro;
			return lnp;
		case ps_end:
			curproc->p_nrlabels = labelcount;
			lnp = newline(OPNO);
			get_off();
			/* Void # localbytes, which we already know
			 * from the PRO instruction.
			 */
			return lnp;
		case ps_mes:
			lnp = arglist(0);
			switch((int) aoff(ARG(lnp),0)) {
			case ms_err:
				error("ms_err encountered");
			case ms_opt:
				error("ms_opt encountered");
			case ms_emx:
				ws = aoff(ARG(lnp),1);
				ps = aoff(ARG(lnp),2);
				break;
			case ms_ext:
				/* this message was already processed
				 * by the lib package
				 */
			case ms_src:
				/* Don't bother about linecounts */
				oldline(lnp);
				return (line_p) 0;
			case ms_par:
				mespar = aoff(ARG(lnp),1);
				/* #bytes of parameters of current proc */
				break;
			}
			return lnp;
		default:
			assert(FALSE);
	}
	/* NOTREACHED */
}
