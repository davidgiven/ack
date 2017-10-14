/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Id$ */

#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include "out.h"

/*

	astrip -- remove symbols and relocation bits

*/

char	temp_name[] = "/tmp/sXXXXXX";
char	*tname;
FILE	*tf;
struct outhead buf;
int	readerror, writeerror;

main(argc, argv)
char **argv;
{
	int	status;

	signal(SIGHUP, SIG_IGN);
	signal(SIGINT, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
	close(mkstemp(temp_name));
	while(--argc) {
		if ((status = strip(argv[argc])) > 1)
			break;
	}
	unlink(temp_name);
	exit(status);
}

strip(name)
char *name;
{
	long size;
	int fw;

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
	fw = open(tname, 2);
	if (fw < 0 || lseek(fw, (long)SZ_HEAD, 0) < 0) {
		fprintf(stderr, "astrip: cannot create temp file %s\n", tname);
		rd_close();
		close(fw);
		return(2);
	}
	if(copy(name, tname, size, rd_fd(), fw)) {
		rd_close();
		close(fw);
		return(1);
	}
	rd_close();
	close(fw);
	size += SZ_HEAD;
	if (! rd_open(tname)) {
		fprintf(stderr, "astrip: cannot read temp file %s\n", tname);
		return(2);
	}
	fw = creat(name, 0777);
	if (fw < 0) {
		fprintf(stderr, "astrip: cannot write %s\n", name);
		rd_close();
		return(1);
	}
	if(copy(tname, name, size, rd_fd(), fw)) {
		close(fw);
		rd_close();
		return(2);
	}

	close(fw);
	rd_close();
	return(0);
}

copy(fnam, tnam, size, fr, fw)
char *fnam;
char *tnam;
long size;
{
	register s, n;
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

rd_fatal()
{
	readerror = 1;
}

wr_fatal()
{
	writeerror = 1;
}
