#ifndef _SYS_MMAN_H
#define _SYS_MMAN_H

#include <sys/types.h>

#define MAP_FAILED ((void *)-1)

#define PROT_NONE	0x00
#define PROT_READ	0x01
#define PROT_WRITE	0x02
#define PROT_EXEC	0x04

#define MAP_PRIVATE	0x0002
#define MAP_FIXED	0x0010
#define MAP_ANON	0x1000

void *mmap(void *, size_t, int, int, int, off_t);
int mprotect(void *, size_t, int);

#endif
