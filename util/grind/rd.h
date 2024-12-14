#ifndef RD_H
#define RD_H

#include <out.h>

#define O_CONVERTED 0x202

extern int rd_open(char* f);
extern void rd_close(void);
extern void rd_ohead(struct outhead* h);
extern void rd_name(struct outname* nm, unsigned int count);
extern void rd_string(char* nm, long count);

#endif
