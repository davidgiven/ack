/*
 * ungetc.c - push a character back onto an imput stream
 */
/* $Header$ */

#include	<stdio.h>
#include	"loc_incl.h"

int
ungetc(int ch, FILE *stream)
{
	unsigned char *p;

	if (ch == EOF  || !io_testflag(stream,_IOREAD))
		return EOF;
	if (stream->_ptr == stream->_buf) {
		if (stream->_count != 0) return EOF;
		stream->_ptr++;
	}
	stream->_count++;
	p = --(stream->_ptr);	/* ??? Bloody vax assembler !!! */
	*p = (unsigned char) ch;
	return ch;
}
