/* $Header$ */

#include "out.h"
#include <signal.h>
#include <stdio.h>

/*

	astrip -- remove symbols and relocation bits

*/

char	*tname;
char	*mktemp();
FILE	*fopen();
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
	tname = mktemp("/tmp/sXXXXX");
	while(--argc) {
		if ((status = strip(argv[argc])) > 1)
			break;
	}
	unlink(tname);
	exit(status);
}

strip(name)
char *name;
{
	long size;

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
	if (writeerror) {
		fprintf(stderr, "astrip: write error on temp file %s\n", tname);
		rd_close();
		wr_close();
		return(1);
	}
	if(copy(name, tname, size)) {
		rd_close();
		wr_close();
		return(1);
	}
	rd_close();
	wr_close();
	size += SZ_HEAD;
	if (! wr_open(name)) {
		fprintf(stderr, "astrip: cannot write %s\n", name);
		return(1);
	}
	if (! rd_open(tname)) {
		fprintf(stderr, "astrip: cannot read temp file %s\n", tname);
		wr_close();
		return(2);
	}
	if(copy(tname, name, size)) {
		wr_close();
		rd_close();
		return(2);
	}

	wr_close();
	rd_close();
	return(0);
}

copy(fnam, tnam, size)
char *fnam;
char *tnam;
long size;
{
	register s, n;
	char lbuf[512];
	int fr, fw;

	fr = rd_fd();
	fw = wr_fd();
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
