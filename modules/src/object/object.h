/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */

#ifndef __OBJECT_INCLUDED__
#define __OBJECT_INCLUDED__

#include <stdio.h>

struct ar_hdr;
struct outhead;
struct outname;
struct outrelo;
struct outsect;
struct ranlib;
struct outname;

int wr_open(const char *filename);
void wr_close(void);
void wr_ohead(const struct outhead *head);
void wr_sect(const struct outsect *sect, unsigned int cnt);
void wr_outsect(int sectno);
void wr_emit(const char *emit, long cnt);
void wr_putc(int c);
void wr_relo(const struct outrelo *relo, unsigned int cnt);
void wr_name(const struct outname *name, unsigned int cnt);
void wr_string(const char *addr, long len);

void wr_arhdr(FILE* fd, struct ar_hdr *arhdr);
void wr_ranlib(FILE* fd, struct ranlib *ran, long cnt);
void wr_int2(FILE* fd, int i);
void wr_int4(FILE* fd, long l);
void wr_bytes(FILE* fd, const char *buf, long cnt);


int rd_open(const char *filename);
int rd_fdopen(FILE* fd);
void rd_close(void);
void rd_ohead(struct outhead* head);
void rd_sect(struct outsect* sect, unsigned int cnt);
void rd_outsect(int sectno);
void rd_emit(char* emit, long cnt);
void rd_relo(struct outrelo* relo, unsigned int cnt);
void rd_rew_relos(struct outhead *head);
void rd_name(struct outname* name, unsigned int cnt);
void rd_string(char* addr, long len);

int rd_arhdr(FILE* fd, register struct ar_hdr *arhdr);
void rd_ranlib(FILE* fd, register struct ranlib *ran, register long cnt);
int rd_int2(FILE* fd);
long rd_int4(FILE* fd);
unsigned int rd_unsigned2(FILE* fd);
void rd_bytes(FILE* fd, char *buf, long l);
FILE* rd_fd(void);

#endif /* __OBJECT_INCLUDED__ */
