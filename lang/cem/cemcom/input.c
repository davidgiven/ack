/* $Header$ */
/*	INPUT AND BUFFER HANDLING MODULE	*/

/*
	[input.c input.h]
	Input buffering module: this module contains the routines that
	offers an input buffering mechanism to the user.

	This module exports the following objects:
	InsertFile() :	suspend input from current buffer and obtain the
			next input characters from the specified file
	InsertText() :	suspend input from current buffer and take the
			specified text as stream of input characters
	LoadChar() :	(defined in input.h) read next character from
			the input ; LoadChar() invokes loadbuf() on
			encounting a ASCII NUL character
	NoUnstack :	if set to non-zero:
			loadbuf() reports "unexpected EOF" on encounting
			the end-of-file or end-of-stacked-text.
	
	Imported objects are:
	IDEPTH, DEBUG, READ_IN_ONE, PATHLENGTH: compile-time parameters
	Malloc(), Salloc(): memory allocation routines
	fatal(), lexerror(): exception handling
	FileName, LineNumber, WorkingDir: input trace for lexical analyser

	READ_IN_ONE DEFINED: every input file is read into memory completely
		and made an input buffer
	READ_IN_ONE NOT DEFINED: the input from files is buffered in
		a fixed length input buffer
*/

#include	"nopp.h"
#include	"inputtype.h"	/* UF */
#include	"interface.h"
#include	"arith.h"
#include	"LLlex.h"
#include	"input.h"
#include	"alloc.h"
#include	"system.h"
#include	"bufsiz.h"

#ifndef NOPP
#include	"idepth.h"	/* UF */
#include	"debug.h"	/* UF */
#include	"pathlength.h"	/* UF */
#include	"assert.h"
#endif NOPP

EXPORT char *ipp = 0;		/* input pointer	*/
EXPORT int NoUnstack = 0;	/* if 1: report EOF	*/

#ifndef READ_IN_ONE
PRIVATE int FilDes = -1;	/* current input medium	*/
#endif READ_IN_ONE

#ifndef NOPP
struct buffer_header	{
	char *bh_name;	/* file name where the text comes from	*/
	unsigned int bh_lineno;
			/* current lineno in file		*/
	long bh_size;	/* = strlen (text), should be unsigned	*/
	char *bh_text;	/* pointer to buffer containing text	*/
	char *bh_ipp;	/* current read pointer (= stacked ipp)	*/
	char *bh_wdir;	/* directory of current file		*/
	int bh_fd;	/* >= 0 (fd if !READ_IN_ONE) in case of file	*/
};

PRIVATE struct buffer_header instack[IDEPTH];	/* stack of input media	*/
PRIVATE struct buffer_header *head  = 0;	/* current input buffer	*/

IMPORT char **WorkingDir;	/* name of current working directory	*/
#else NOPP
long isize;
char ibuf[BUFSIZ];
#endif NOPP

#ifdef READ_IN_ONE
/*	readfile() creates a buffer in which the text of the file
	is situated.  A pointer to the start of this text is
	returned.  *size is initialized with the buffer length.
	Note that the file input buffer is prepared for the
	preprocessor by inserting a '\n' in the beginning of the
	text and appending a '\n' at the end of the text.  The
	file text start at position 1 of the input buffer. This is
	done to allow pushback.
*/

PRIVATE char *
readfile(filename, size)
	char *filename;
	long *size;
{
	int fd;			/* filedescriptor for `filename'	*/
	char *cbuf;		/* pointer to buffer to be returned	*/
	register tmp;

	if ((fd = sys_open(filename, OP_RDONLY)) < 0)	{
		/* can't open this file */
		return (char *) 0;
	}

	if ((*size = sys_fsize(fd)) < 0)
		fatal("(readfile) cannot get size of file");

	/* allocate enough space to store contents of the file		*/
	cbuf = Malloc(*size + 2);

	tmp = sys_read(fd, cbuf + 1, (int) *size); /* read the file	*/
	if (tmp != *size)
		fatal("(readfile) bad read count");

	(*size)++;		/* keep book of the size!	*/
	sys_close(fd);		/* filedes no longer needed	*/
	cbuf[0] = '\0';		/* allow pushback of first char	*/
	cbuf[*size] = '\0';	/* invoke loadbuf() at end	*/
	return cbuf;
}
#endif READ_IN_ONE

#ifndef NOPP
#ifndef READ_IN_ONE
/*	Input buffer supplying routines: pushbuf() and popbuf()
*/
PRIVATE char *bufstack[IDEPTH] = 0;
PRIVATE bufstptr = 0;

PRIVATE char *
pushbuf()
{
	if (bufstptr >= IDEPTH)
		fatal("ran out of input buffers");
	if (bufstack[bufstptr] == 0) {
		bufstack[bufstptr] = Malloc(BUFSIZ + 4);
	}
	return bufstack[bufstptr++];
}

PRIVATE
popbuf()
{
	bufstptr--;
	ASSERT(bufstptr >= 0);
}
#endif READ_IN_ONE
#endif NOPP

#ifndef NOPP
/*	Input buffer administration: push_bh() and pop_bh()
*/
PRIVATE struct buffer_header *
push_bh()
{
	if (head) {
		if (head >= &instack[IDEPTH - 1])
			fatal("too many nested input texts");
		head->bh_ipp = ipp;
		head->bh_lineno = LineNumber;
		head++;
	}
	else
		head = &instack[0];

	return head;
}
#endif NOPP

#ifndef NOPP
/*	pop_bh() uncovers the previous inputbuffer on the stack
	of headers.  0 is returned if there are no more
	inputbuffers on the stack, 1 is returned in the other case.
*/
PRIVATE int
pop_bh()
{
	int pfd = head->bh_fd;

	if (NoUnstack) {
		lexerror("unexpected EOF");
	}

	if (head <= &instack[0])	{	/* no more entries	*/
		head = (struct buffer_header *) 0;
		return 0;
	}

	ipp = (--head)->bh_ipp; /* restore the previous input pointer	*/

	if (pfd >= 0)	{		/* unstack a file	*/
#ifndef READ_IN_ONE
		closefile(pfd);
		popbuf();		/* free last buffer	*/
#endif READ_IN_ONE
		LineNumber = head->bh_lineno;
		FileName = head->bh_name;
		*WorkingDir = head->bh_wdir;
	}

#ifndef READ_IN_ONE
	FilDes = head->bh_fd;
#endif READ_IN_ONE

	return 1;
}
#endif NOPP

#ifndef READ_IN_ONE
/*	low level IO routines: openfile(), readblock() and closefile()
*/

PRIVATE int
openfile(filename)
	char *filename;
{
	int fd;			/* filedescriptor for `filename'	*/

	if ((fd = sys_open(filename, OP_RDONLY)) < 0 && sys_errno == EMFILE)
		fatal("too many files open");
	return fd;
}

PRIVATE
closefile(fd)
{
	sys_close(fd);
}

PRIVATE int
readblock(fd, buf)
	char buf[];
{
	register n;

	if ((n = sys_read(fd, &buf[1], BUFSIZ)) < 0) {
		fatal("(readblock) bad read from file");
	}
	buf[0] = buf[n + 1] = '\0';
	return n;
}
#endif READ_IN_ONE

/*	Interface routines : InsertFile(), InsertText() and loadbuf()
*/

EXPORT int
InsertFile(filnam, table)
	char *filnam;
	char *table[];
{
	char *mk_filename(), *newfn;
	char *strcpy();

#ifdef READ_IN_ONE
	char *readfile(), *text;
	long size;
#else READ_IN_ONE
	int fd = -1;
#endif READ_IN_ONE

	if (!filnam)
		return 0;

#ifndef NOPP
	if (table == 0 || filnam[0] == '/') {	/* don't look in the table! */
#endif NOPP
#ifdef READ_IN_ONE
		text = readfile(filnam, &size);
#else READ_IN_ONE
		fd = openfile(filnam);
#endif READ_IN_ONE
#ifndef NOPP
	}
	else {
		while (*table) {	/* look in the directory table	*/
			newfn = mk_filename(*table++, filnam);
#ifdef READ_IN_ONE
			if (text = readfile(newfn, &size))
#else READ_IN_ONE
			if ((fd = openfile(newfn)) >= 0)
#endif READ_IN_ONE
			{
				/* free filnam ??? */
				filnam = Salloc(newfn, strlen(newfn) + 1);
				break;
			}
		}
	}
#endif NOPP

#ifdef READ_IN_ONE
	if (text)
#else READ_IN_ONE
	if (fd >= 0)
#endif READ_IN_ONE
#ifndef NOPP
	{
		struct buffer_header *push_bh();
		register struct buffer_header *bh = push_bh();

		setwdir(WorkingDir, filnam);
		bh->bh_lineno = LineNumber = 0;
		bh->bh_name = FileName = filnam;
		bh->bh_wdir = *WorkingDir;
#ifdef READ_IN_ONE
		bh->bh_size = size;
		bh->bh_fd = 0;		/* this is a file */
		ipp = bh->bh_text = text;
#else READ_IN_ONE
		bh->bh_size = readblock(fd, ipp = bh->bh_text = pushbuf()) + 1;
		FilDes = bh->bh_fd = fd;
#endif READ_IN_ONE
		bh->bh_text[0] = '\n';	/* wake up pp if '#' comes first */
		return 1;
	}
#else NOPP
	{
#ifdef READ_IN_ONE
		isize = size;
		ipp = text;
#else READ_IN_ONE
		isize = readblock(FilDes = fd, ipp = &ibuf[0]) + 1;
#endif READ_IN_ONE
		ibuf[0] = '\n';
		return 1;
	}
#endif NOPP
	return 0;
}

#ifndef NOPP
EXPORT
InsertText(text, length)
	char *text;
{
	struct buffer_header *push_bh();
	register struct buffer_header *bh = push_bh();

	bh->bh_name = FileName;
	bh->bh_lineno = LineNumber;
	bh->bh_size = (long) length;
	bh->bh_text = text;
	bh->bh_wdir = *WorkingDir;
	bh->bh_fd = -1;			/* this is no file !	*/
	ipp = text + 1;
#ifndef READ_IN_ONE
	FilDes = -1;
#endif READ_IN_ONE
}
#endif NOPP

/*	loadbuf() is called if LoadChar meets a '\0' character
	which may be the end-of-buffer mark of the current input
	buffer.  The '\0' could be genuine although not likely.
	Note: this routine is exported due to its occurence in the definition
	of LoadChar [input.h], that is defined as a macro.
*/
EXPORT int
loadbuf()
{
#ifndef NOPP
	if (!head) {
		/* stack exhausted, EOF on sourcefile	*/
		return EOI;
	}
#endif NOPP
	
#ifndef NOPP
	if (ipp < &(head->bh_text[head->bh_size]))
#else NOPP
	if (ipp < &ibuf[isize])
#endif NOPP
	{
		/* a genuine '\0' character has been seen	*/
		return '\0';
	}

#ifndef READ_IN_ONE
#ifndef NOPP
	if (	FilDes >= 0
	&&	(head->bh_size = readblock(FilDes, head->bh_text)) > 0
	)	{
		return ipp = &(head->bh_text[1]), *ipp++;
	}
#else NOPP
	if (FilDes >= 0 && (isize = readblock(FilDes, &ibuf[0])) > 0)
		return ipp = &ibuf[1], *ipp++;
#endif NOPP

#endif READ_IN_ONE

#ifdef NOPP
	if (NoUnstack)
		lexerror("unexpected EOF");
#ifndef READ_IN_ONE
	closefile(FilDes);
#endif READ_IN_ONE
#endif NOPP

	return
#ifndef NOPP
		pop_bh() ? (*ipp ? *ipp++ : loadbuf()) :
#endif NOPP
		(ipp = &"\0\0"[1], EOI);
}

/*	Some miscellaneous routines : setwdir() and mk_filename()
*/

#ifndef NOPP
/*	setwdir() updates *wdir according to the old working
	directory (*wdir) and the filename fn, which may contain
	some path name.  The algorithm used here is:
	setwdir(DIR, FILE):
		if (FILE == "/***")
			*DIR = "/"
		else
		if (contains(FILE, '/'))
			*DIR = directory(FILE)
		else
			*DIR remains unchanged
*/
PRIVATE
setwdir(wdir, fn)
	char *fn, **wdir;
{
	register char *p;
	char *rindex();

	p = rindex(fn, '/');
	while (p && *(p + 1) == '\0') {	/* remove trailing /'s */
		*p = '\0';
		p = rindex(fn, '/');
	}

	if (fn[0] == '\0' || (fn[0] == '/' && p == &fn[0])) /* absolute path */
		*wdir = "/";
	else
	if (p) {
		*p = '\0';
		*wdir = Salloc(fn, p - &fn[0] + 1);
		*p = '/';
	}
}
#endif NOPP

#ifndef NOPP
/*	mk_filename() concatenates a dir and filename.
*/
PRIVATE char *
mk_filename(dir, file)
	register char *dir, *file;
{
	static char newfn[PATHLENGTH];
	register char *dst = &newfn[0];

	if (!(dir[0] == '.' && dir[1] == '\0')) {
		while (*dst++ = *dir++);
		*(dst - 1) = '/';
	}
	while (*dst++ = *file++);
	return &newfn[0];
}
#endif NOPP
