#include <stdio.h>
#include "param.h"
#include "types.h"
#include "assert.h"
#include "../../h/em_pseu.h"
#include "../../h/em_spec.h"
#include "../../h/em_mes.h"
#include "lookup.h"
#include "ext.h"

/*
 * (c) copyright 1983 by the Vrije Universiteit, Amsterdam, The Netherlands.
 *
 *          This product is part of the Amsterdam Compiler Kit.
 *
 * Permission to use, sell, duplicate or disclose this software must be
 * obtained in writing. Requests for such permissions may be sent to
 *
 *      Dr. Andrew S. Tanenbaum
 *      Wiskundig Seminarium
 *      Vrije Universiteit
 *      Postbox 7161
 *      1007 MC Amsterdam
 *      The Netherlands
 *
 * Author: Hans van Staveren
 */


cleanup() {
	FILE *infile;
	register c;
	register sym_p *spp,sp;

 	for (spp=symhash;spp< &symhash[NSYMHASH];spp++)
 		for (sp = *spp; sp != (sym_p) 0; sp = sp->s_next)
 			if ((sp->s_flags & SYMOUT) == 0)
 				outdef(sp);
	if(!Lflag)
		return;
	c=fclose(outfile);
	assert(c != EOF);
	outfile = stdout;
	infile = fopen(template,"r");
	if (infile == NULL)
		error("temp file disappeared");
	outshort(sp_magic);
	outinst(ps_mes);
	outint(ms_ext);
	for (spp=symhash;spp< &symhash[NSYMHASH];spp++)
		for (sp = *spp; sp != (sym_p) 0; sp = sp->s_next)
			if ((sp->s_flags&(SYMDEF|SYMGLOBAL)) == (SYMDEF|SYMGLOBAL))
				outsym(sp);
	putc(sp_cend,outfile);
	while ( (c=getc(infile)) != EOF)
		putc(c,outfile);
	c=fclose(infile);
	assert(c != EOF);
	c=unlink(template);
	assert(c == 0);
}
