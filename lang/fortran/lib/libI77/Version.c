static char junk[] = "\n@(#) LIBI77 VERSION pjw,dmg-mods 16 May 1991\n";

/*
2.01	$ format added
2.02	Coding bug in open.c repaired
2.03	fixed bugs in lread.c (read * with negative f-format) and lio.c
	and lio.h (e-format conforming to spec)
2.04	changed open.c and err.c (fopen and freopen respectively) to
	update to new c-library (append mode)
2.05	added namelist capability
2.06	allow internal list and namelist I/O
*/

/*
close.c:
	allow upper-case STATUS= values
endfile.c
	create fort.nnn if unit nnn not open;
	else if (file length == 0) use creat() rather than copy;
	use local copy() rather than forking /bin/cp;
	rewind, fseek to clear buffer (for no reading past EOF)
err.c
	use neither setbuf nor setvbuf; make stderr buffered
fio.h
	#define _bufend
inquire.c
	upper case responses;
	omit byfile test from SEQUENTIAL=
	answer "YES" to DIRECT= for unopened file (open to debate)
lio.c
	flush stderr, stdout at end of each stmt
	space before character strings in list output only at line start
lio.h
	adjust LEW, LED consistent with old libI77
lread.c
	use atof()
	allow "nnn*," when reading complex constants
open.c
	try opening for writing when open for read fails, with
	special uwrt value (2) delaying creat() to first write;
	set curunit so error messages don't drop core;
	no file name ==> fort.nnn except for STATUS='SCRATCH'
rdfmt.c
	use atof(); trust EOF == end-of-file (so don't read past
	end-of-file after endfile stmt)
sfe.c
	flush stderr, stdout at end of each stmt
wrtfmt.c:
	use upper case
	put wrt_E and wrt_F into wref.c, use sprintf()
		rather than ecvt() and fcvt() [more accurate on VAX]
*/

/* 16 Oct. 1988: uwrt = 3 after write, rewind, so close won't zap the file. */

/* 10 July 1989: change _bufend to buf_end in fio.h, wsfe.c, wrtfmt.c */

/* 28 Nov. 1989: corrections for IEEE and Cray arithmetic */
/* 29 Nov. 1989: change various int return types to long for f2c */
/* 30 Nov. 1989: various types from f2c.h */
/*  6 Dec. 1989: types corrected various places */
/* 19 Dec. 1989: make iostat= work right for internal I/O */
/*  8 Jan. 1990: add rsne, wsne -- routines for handling NAMELIST */
/* 28 Jan. 1990: have NAMELIST read treat $ as &, general white
		 space as blank */
/* 27 Mar. 1990: change an = to == in rd_L(rdfmt.c) so formatted reads
		 of logical values reject letters other than fFtT;
		 have nowwriting reset cf */
/* 14 Aug. 1990: adjust lread.c to treat tabs as spaces in list input */
/* 17 Aug. 1990: adjust open.c to recognize blank='Z...' as well as
		 blank='z...' when reopening an open file */
/* 30 Aug. 1990: prevent embedded blanks in list output of complex values;
		 omit exponent field in list output of values of
		 magnitude between 10 and 1e8; prevent writing stdin
		 and reading stdout or stderr; don't close stdin, stdout,
		 or stderr when reopening units 5, 6, 0. */
/* 18 Sep. 1990: add component udev to unit and consider old == new file
		 iff uinode and udev values agree; use stat rather than
		 access to check existence of file (when STATUS='OLD')*/
/* 2 Oct. 1990:  adjust rewind.c so two successive rewinds after a write
		 don't clobber the file. */
/* 9 Oct. 1990:  add #include "fcntl.h" to endfile.c, err.c, open.c;
		 adjust g_char in util.c for segmented memories. */
/* 17 Oct. 1990: replace abort() and _cleanup() with calls on
		 sig_die(...,1) (defined in main.c). */
/* 5 Nov. 1990:  changes to open.c: complain if new= is specified and the
		 file already exists; allow file= to be omitted in open stmts
		 and allow status='replace' (Fortran 90 extensions). */
/* 11 Dec. 1990: adjustments for POSIX. */
/* 15 Jan. 1991: tweak i_ungetc in rsli.c to allow reading from
		 strings in read-only memory. */
/* 25 Apr. 1991: adjust namelist stuff to work with f2c -i2 */
/* 26 Apr. 1991: fix some bugs with NAMELIST read of multi-dim. arrays */
/* 16 May 1991:  increase LEFBL in lio.h to bypass NeXT bug */
