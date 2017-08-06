/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */

#ifndef __OBJECT_INCLUDED__
#define __OBJECT_INCLUDED__

struct ar_hdr;
struct outhead;
struct outrelo;
struct outsect;
struct ranlib;

int wr_open(char *f);
void wr_close(void);
void wr_ohead(struct outhead *h);
void wr_sect(struct outsect *s, unsigned int c);
void wr_outsect(int sectno);
void wr_emit(char *b, long c);
void wr_putc(int c);
void wr_relo(struct outrelo *r, unsigned int c);
void wr_name(struct outname *n, unsigned int c);
void wr_string(char *s, long c);
void wr_arhdr(int fd, struct ar_hdr *a);
void wr_ranlib(int fd, struct ranlib *r, long cnt);
void wr_int2(int fd, int i);
void wr_long(int fd, long l);
void wr_bytes(int fd, char *buf, long l);
int rd_open(char *f);
int rd_fdopen(int f);
void rd_close(void);
void rd_ohead(struct outhead *h);
void rd_sect(struct outsect *s, unsigned int c);
void rd_outsect(int sectno);
void rd_emit(char *b, long c);
void rd_relo(struct outrelo *r, unsigned int c);
void rd_rew_relos(struct outhead *head);
void rd_name(struct outname *n, unsigned int c);
void rd_string(char *s, long c);
int rd_arhdr(int fd, struct ar_hdr *a);
void rd_ranlib(int fd, struct ranlib *r, long cnt);
int rd_int2(int fd);
long rd_long(int fd);
void rd_bytes(int fd, char *buf, long l);
int rd_fd(void);

#endif /* __OBJECT_INCLUDED__ */
