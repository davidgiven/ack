/* $Header$ */
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
 */

#include	<em_abs.h>
#include	<em_path.h>
#include	<pc_file.h>

#define	MESLEN		30
#define PATHLEN		100

/* to make it easier to patch ... */
char 			emdir[64] = EM_DIR;
extern struct file	*_curfil;

extern int		_pargc;
extern char		**_pargv;
extern char		**_penvp;

extern char		*_hol0();
extern			_trp();
extern			exit();
extern int		open();
extern int		read();
extern int		write();

/* Modified not to use a table of indices any more. This circumvents yet 
   another point where byte order in words would make you lose.
 */

_catch(erno) unsigned erno; {
	char		*p,*q,**qq;
	unsigned	i;
	int		fd;
	char		*pp[8];
	char		mes[MESLEN];
	char		filename[PATHLEN];
	char		c;

	qq = pp;
	if (p = FILN)
		*qq++ = p;
	else
		*qq++ = _pargv[0];
	p = &("xxxxx: "[5]);
	if (i = LINO) {
		*qq++ = ", ";
		do
			*--p = i % 10 + '0';
		while (i /= 10);
	}
	*qq++ = p;
	if ((erno & ~037) == 0140 && (_curfil->flags&0377)==MAGIC) { 
		/* file error */
		*qq++ = "file ";
		*qq++ = _curfil->fname;
		*qq++ = ": ";
	}
	if ( (i=strtobuf(emdir,filename,PATHLEN)) >= PATHLEN-1 ||
	     (filename[i]='/' ,
	      strtobuf(RTERR_PATH,filename+i+1,PATHLEN-i-1) >= PATHLEN-i-1
	     ) )
		goto error;
	if ((fd=open(filename,0))<0)
		goto error;
	/* skip to correct message */
	for(i=0;i<erno;i++)
		do if (read(fd,&c,1)!=1)
			goto error;
		while (c!= '\n');
	if(read(fd,mes,MESLEN-1)<=0)
		goto error;
	mes[MESLEN-1]=0;
	for(i=0;i<MESLEN-1;i++)
		if(mes[i]=='\n')
			mes[i+1]=0;
	*qq++ = mes;
	*qq = 0;
	qq = pp;
	while (q = *qq++) {
		p = q;
		while (*p)
			p++;
		if (write(2,q,p-q) < 0)
			;
	}
	exit(erno);
error:
	_trp(erno);
}
