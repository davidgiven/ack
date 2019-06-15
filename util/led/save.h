#ifndef __SAVE_H_INCLUDED__
#define __SAVE_H_INCLUDED__

#include "memory.h"

struct ar_hdr;
struct outname;

void savemagic(void);
void savehdr(struct ar_hdr *hdr);
ind_t savechar(int piece, ind_t off);
void savelocal(struct outname *name);

#endif /* __SAVE_H_INCLUDED__ */
