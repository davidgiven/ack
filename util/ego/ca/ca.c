/*
 *  C O M P A C T   A S S E M B L Y   L A N G U A G E   G E N E R A T I O N
 *
 */


#include <stdio.h>
#include "../share/types.h"
#include "ca.h"
#include "../share/debug.h"
#include "../share/global.h"
#include "../share/lset.h"
#include "../share/files.h"
#include "../share/map.h"
#include "../share/alloc.h"
#include "../share/get.h"
#include "ca_put.h"


/* This phase transforms the Intermediate Code of the global optimizer
 * to 'standard' compact assembly language, which will be processed
 * by the code generator.
 */


short	dlength;
dblock_p *dmap;

char **dnames, **pnames;  /* Dynamically allocated arrays of strings.
			 * pnames[i] contains a pointer to the name
			 * of the procedure  with proc_id i.
			 */



STATIC int makedmap(dbl)
	dblock_p dbl;
{
	/* construct the dmap table */

	dblock_p d;
	int cnt;

	/* determine the length of the table */

	cnt = 0;
	for (d = dbl; d != (dblock_p) 0; d = d->d_next) cnt++;
	dmap = (dblock_p *) newmap(cnt);
	for (d = dbl; d != (dblock_p) 0; d = d->d_next) {
		assert(d->d_id) <= cnt;
		dmap[d->d_id] = d;
	}
	return cnt;
}



STATIC getdnames(dumpd)
	FILE *dumpd;
{
	/* Read the names of the datalabels from
	 * the dump file.
	 */

	char str[IDL+1];
	char *s;
	int id;
	register int i;

	dnames = (char **) newnametab(dlength,IDL);
	for (;;) {
		if (fscanf(dumpd,"%d	%s",&id,str) == EOF) return;
		assert(id <= dlength);
		s = dnames[id];
		for (i = 0; i < IDL; i++) {
			*s++ = str[i];
		}
	}
}

STATIC getpnames(dumpp)
	FILE *dumpp;
{
	/* Read the names of the procedures from
	 * the dump file.
	 */

	char str[IDL+1];
	char *s;
	int id;
	register int i;

	pnames = (char **) newnametab(plength,IDL);
	for (;;) {
		if (fscanf(dumpp,"%d	%s",&id,str) == EOF) return;
		assert(id <= plength);
		s = pnames[id];
		for (i = 0; i < IDL; i++) {
			*s++ = str[i];
		}
	}
}


STATIC bool name_exists(name,endp,endd)
	char *name;
	proc_p endp;
	dblock_p endd;
{
	/* Search the proctable (from fproc to endp)
	 * and the data block table (from fdblock to endd)
	 * to see if the name is already in use.
	 */

	proc_p p;
	dblock_p d;

	for (p = fproc; p != endp; p = p->p_next) {
		if (strncmp(name,pnames[p->p_id],IDL) == 0) return TRUE;
	}
	for (d = fdblock; d != endd; d = d->d_next) {
		if (strncmp(name,dnames[d->d_id],IDL) == 0) return TRUE;
	}
	return FALSE;
}



static int nn = 0;

STATIC new_name(s)
	char *s;
{
	s[0] = '_';
	s[1] = 'I';
	s[2] = 'I';
	sprintf(&s[3],"%d",nn);
	nn++;
}


	
STATIC uniq_names()
{
	/* The names of all internal procedures and data blocks
	 * are made different. As the optimizer combines several
	 * modules into one, there may be name conflicts between
	 * procedures or data blocks that were internal in
	 * different source modules.
	 */

	proc_p p;
	dblock_p d;

	for (p = fproc; p != (proc_p) 0; p = p->p_next) {
		if (!(p->p_flags1 & PF_EXTERNAL) &&
		    name_exists(pnames[p->p_id],p,fdblock)) {
			new_name(pnames[p->p_id]);
		}
	}
	for (d = fdblock; d != (dblock_p) 0; d = d->d_next) {
		if (!(d->d_flags1 & DF_EXTERNAL) &&
		    name_exists(dnames[d->d_id],(proc_p) 0,d) ) {
			new_name(dnames[d->d_id]);
		}
	}
}
main(argc,argv)
	int argc;
	char *argv[];
{
	/* CA does not output proctable etc. files. Instead, its
	 * pname2 and dname2 arguments contain the names of the
	 * dump files created by IC.
	 */
	FILE *f, *f2;	  /* The EM input and output. */
	FILE *df, *pf;    /* The dump files */
	line_p lnp;

	fproc = getptable(pname); /* proc table */
	fdblock = getdtable(dname);  /* data block table */
	dlength = makedmap(fdblock); /* allocate dmap table */
	df = openfile(dname2,"r");
	getdnames(df);
	fclose(df);
	pf = openfile(pname2,"r");
	getpnames(pf);
	fclose(pf);
	uniq_names();
	f = openfile(lname,"r");
	f2 = stdout;
	cputmagic(f2); /* write magic number */
	while ((lnp = get_ca_lines(f,&curproc)) != (line_p) 0) {
		cputlines(lnp,f2);
	}
	fclose(f);
	fclose(f2);
	exit(0);
}
