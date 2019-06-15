#ifndef ACK_FILE_H
#define ACK_FILE_H

/* CP/M custom FILE* implementation. */

#define L_tmpnam 16 /* 0a:12345678.abc */
#define ACK_TMP_PREFIX "0a:tmp"
#define TMP_MAX 999

#include <cpm.h>

#define CPM_FAKE_FILE ((FILE*)1)
#define stdin CPM_FAKE_FILE
#define stdout CPM_FAKE_FILE
#define stderr CPM_FAKE_FILE

struct FILE
{
    uint8_t user;
    FCB fcb;
    char buffer[128];
};

#define getc(p) cpm_getc(p)
#define putc(c, p) cpm_putc(c, p)
#define feof(p) cpm_feof(p)
#define ferror(p) cpm_ferror(p)
#define clearerr(p) cpm_clearerr(p)

extern int cpm_putc(int c, FILE* stream);
extern int cpm_getc(FILE* stream);

#if 0
#define	getc(p)		(--(p)->_count >= 0 ? (int) (*(p)->_ptr++) : \
				__fillbuf(p))
#define	putc(c, p)	(--(p)->_count >= 0 ? \
			 (int) (*(p)->_ptr++ = (c)) : \
			 __flushbuf((c),(p)))

#define	feof(p)		(((p)->_flags & _IOEOF) != 0)
#define	ferror(p)	(((p)->_flags & _IOERR) != 0)
#define clearerr(p)	((p)->_flags &= ~(_IOERR|_IOEOF))
#endif


#endif
