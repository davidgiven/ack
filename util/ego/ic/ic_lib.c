/* $Header$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/*  I N T E R M E D I A T E   C O D E
 *
 *  I C _ L I B . C
 */


#include <stdio.h>
#include <em_spec.h>
#include <em_pseu.h>
#include <em_mes.h>
#include <arch.h>
#include "../share/types.h"
#include "../share/debug.h"
#include "ic.h"
#include "ic_lookup.h"
#include "ic_io.h"
#include "../share/global.h"
#include "../share/files.h"
#include "ic_lib.h"


STATIC skip_string(n)
	offset n;
{
	/* Read a string of length n and void it */

	while (n--) {
		readchar();
	}
}


STATIC skip_arguments()
{
	/* Skip the arguments of a MES pseudo. The argument
	 * list is terminated by a sp_cend byte.
	 */

	for (;;) {
		switch(table2()) {
			case sp_scon:
				get_off(); /* void */
				/* fall through !!! */
			case sp_icon:
			case sp_ucon:
			case sp_fcon:
				get_int(); /* void */
				skip_string(get_off());
				break;
			case sp_cend:
				return;
			default:
				break;
		}
	}
}



STATIC bool proc_wanted(name)
	char *name;
{
	/* See if 'name' is the name of an external procedure
	 * that has been used before, but for which no body
	 * has been given so far.
	 */

	proc_p p;

	if (( p = proclookup(name,IMPORTING)) != (proc_p) 0 &&
	   !(p->p_flags1 & PF_BODYSEEN)) {
		return TRUE;
	} else {
		return FALSE;
	}
}



STATIC bool data_wanted(name)
	char *name;
{
	/* See if 'name' is the name of an externally visible
	 * data block that has been used before, but for which
	 * no defining occurrence has been given yet.
	 */

	dblock_p db;

	if ((db = symlookup(name,IMPORTING)) != (dblock_p) 0 &&
	   db->d_pseudo == DUNKNOWN) {
		return TRUE;
	} else {
		return FALSE;
	}
}



STATIC bool wanted_names()
{
	/* Read the names of procedures and data labels,
	 * appearing in a 'MES ms_ext' pseudo. Those are
	 * the names of entities that are imported by
	 * a library module.
	 * If any of them is wanted, return TRUE.
	 * A name is wanted if it is the name of a procedure
	 * or data block for which applied occurrences but
	 * no defining occurrence has been met.
	 */

	for (;;) {
		switch(table2()) {
			case DLBX:
				if (data_wanted(string)) {
					return TRUE;
				}
				/* A data entity with the name
				 * string is available.
				 */
				break;
			case sp_pnam:
				if (proc_wanted(string)) {
					return TRUE;
				}
				break;
			case sp_cend:
				return FALSE;
			default:
				error("wrong argument of MES %d", ms_ext);
		}
	}
}



STATIC FILE *curfile = NULL;
STATIC bool useful()
{
	/* Determine if any entity imported by the current
	 * compact EM assembly file  (which will usually be
	 * part of an archive file) is useful to us.
	 * The file must contain (before any other non-MES line)
	 * a 'MES ms_ext' pseudo that has as arguments the names
	 * of the entities imported.
	 */

	for (;;) {
		if (table1() != PSEU || tabval != ps_mes) {
			error("cannot find MES %d in library file",ms_ext);
		}
		if (table2() != CSTX1) {
			error("message number expected");
		}
		if (tabval == ms_ext) {
			/* This is the one we searched */
			return wanted_names();
			/* Read the names of the imported entities
			 * and check if any of them is wanted.
			 */
		} else {
			skip_arguments(); /* skip remainder of this MES */
		}
	}
}



STATIC bool is_archive(name)
	char *name;
{
	/* See if 'name' is the name of an archive file, i.e. it
	 * should end on ".ma" and should at least be four characters
	 * long (i.e. the name ".ma" is not accepted as an archive name!).
	 */

	register char *p;

	for (p = name; *p; p++);
	return (p > name+3) && (*--p == 'a') && (*--p == 'm') && (*--p == '.');
}



STATIC struct ar_hdr hdr;

STATIC bool read_hdr()
{
	/* Read the header of an archive module */

	char buf[AR_TOTAL];
	register char *c = buf;
	register char *p = hdr.ar_name;
	register int i;

	fread(c, AR_TOTAL, 1, curfile);
	if (feof(curfile)) return 0;
	i = 14;
	while (i--) {
		*p++ = *c++;
	}

#define get2(c)        (((c)[0]&0377) | ((unsigned) ((c)[1]&0377) << 8))

	hdr.ar_date = ((long) get2(c)) << 16; c += 2;
	hdr.ar_date |= ((long) get2(c)) & 0xffff; c += 2;
	hdr.ar_uid = *c++;
	hdr.ar_gid = *c++;
	hdr.ar_mode = get2(c); c += 2;
	hdr.ar_size = (long) get2(c) << 16; c += 2;
	hdr.ar_size |= (long) get2(c) & 0xffff;

	return 1;
}



STATIC int argcnt = ARGSTART - 1;
STATIC short arstate = NO_ARCHIVE;


FILE *next_file(argc,argv)
	int argc;
	char *argv[];
{
	/* See if there are more EM input files. The file names
	 * are given via argv. If a file is an archive file
	 * it is supposed to be a library of EM compact assembly
	 * files. A module (file) contained in this archive file
	 * is only used if it imports at least one procedure or
	 * datalabel for which we have not yet seen a defining
	 * occurrence, although we have seen a used occurrence.
	 */

	 long ptr;

	 for (;;) {
		/* This loop is only exited via a return */
		if (arstate == ARCHIVE) {
			/* We were reading an archive file */
			if (ftell(curfile) & 1) {
				/* modules in an archive file always
				 * begin on a word boundary, i.e. at
				 * an even address.
				 */
				fseek(curfile,1L,1);
			}
			if (read_hdr()) { /* read header of next module */
				ptr = ftell(curfile); /* file position */
				file_init(curfile,ARCHIVE,hdr.ar_size);
				/* tell i/o package that we're reading
				 * an archive module of given length.
				 */
				if (useful()) {
					/* re-initialize file, because 'useful'
					 * has read some bytes too.
					 */
					fseek(curfile,ptr,0); /* start module */
					file_init(curfile,ARCHIVE,hdr.ar_size);
					return curfile;
				} else {
					/* skip this module */
					fseek(curfile,
					  ptr+hdr.ar_size,0);
				}
			} else {
				/* done with this archive */
				arstate = NO_ARCHIVE;
			}
		} else {
			/* open next file, close old */
			if (curfile != NULL) {
				fclose(curfile);
			}
			argcnt++;
			if (argcnt >= argc) {
				/* done with all arguments */
				return NULL;
			}
			filename = argv[argcnt];
			if ((curfile = fopen(filename,"r")) == NULL) {
				error("cannot open %s",filename);
			}
			if (is_archive(filename)) {
				/* ends on '.ma' */
				arstate = ARCHIVE;
				arch_init(curfile); /* read magic ar number */
			} else {
				file_init(curfile,NO_ARCHIVE,0L);
				return curfile;
			}
		}
	}
}
