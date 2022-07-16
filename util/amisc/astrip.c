/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Id$ */

#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include "system.h"
#include "object.h"
#include "out.h"

/*

	astrip -- remove symbols and relocation bits

*/

char	tname[L_tmpnam];
FILE	*tf;
struct outhead buf;
int	readerror, writeerror;


static int copy(char *, char *, long, FILE *, FILE *);
static int strip(char *);

int main(int argc, char **argv)
{
	int	status;

	if (sys_tmpnam(tname)==NULL)
	{
		fprintf(stderr, "astrip: cannot create temporary filename\n");
		return(1);
	}
	fclose(fopen(tname,"wb"));
	while(--argc) {
		if ((status = strip(argv[argc])) > 1)
			break;
	}
	remove(tname);
	return status;
}

int strip(char *name)
{
	long size;
	FILE *fw;

	if (! rd_open(name)) {
		fprintf(stderr, "astrip: cannot open %s\n", name);
		return(1);
	}
	readerror = 0;
	writeerror = 0;
	rd_ohead(&buf);
	if(readerror || BADMAGIC(buf)) {
		fprintf(stderr, "astrip: %s-- bad format\n", name);
		rd_close();
		return(1);
	}
	size = OFF_RELO(buf) - SZ_HEAD;
	buf.oh_flags &= ~HF_LINK;
	buf.oh_nrelo = 0;
	buf.oh_nname = 0;
	buf.oh_nchar = 0;


	if (! wr_open(tname)) {
		fprintf(stderr, "astrip: cannot create temp file %s\n", tname);
		rd_close();
		return(2);
	}
	wr_ohead(&buf);
	wr_close();
	if (writeerror) {
		fprintf(stderr, "astrip: write error on temp file %s\n", tname);
		rd_close();
		return(1);
	}
	fw = fopen(tname, "ab");
	if ((fw == NULL) || (fseek(fw, (long)SZ_HEAD, SEEK_SET)!=0)) {
		fprintf(stderr, "astrip: cannot create temp file %s\n", tname);
		rd_close();
		fclose(fw);
		return(2);
	}
	if(copy(name, tname, size, rd_fd(), fw)) {
		rd_close();
		fclose(fw);
		return(1);
	}
	rd_close();
	fclose(fw);
	size += SZ_HEAD;
	if (! rd_open(tname)) {
		fprintf(stderr, "astrip: cannot read temp file %s\n", tname);
		return(2);
	}
	fw = fopen(name, "wb");
	if (fw == NULL) {
		fprintf(stderr, "astrip: cannot write %s\n", name);
		rd_close();
		return(1);
	}
	if(copy(tname, name, size, rd_fd(), fw)) {
		fclose(fw);
		rd_close();
		return(2);
	}
	fclose(fw);
	rd_close();
	/* Change the mode to everything. */
	chmod(name,S_IRWXU | S_IRWXG | S_IRWXO);
	return(0);
}

static int copy(char *fnam, char *tnam, long size, FILE *fr, FILE *fw)
{
	register int s;
	char lbuf[512];

	while(size != (long)0) {
		s = 512;
		if(size < 512)
			s = (int) size;
		rd_bytes(fr, lbuf, (long) s);
		if (readerror) {
			fprintf(stderr, "astrip: unexpected eof on %s\n", fnam);
			return(1);
		}
		wr_bytes(fw, lbuf, (long) s);
		if (writeerror) {
			fprintf(stderr, "astrip: write error on %s\n", tnam);
			return(1);
		}
		size -= (long)s;
	}
	return(0);
}

void rd_fatal(void)
{
	readerror = 1;
}

void wr_fatal(void)
{
	writeerror = 1;
}
