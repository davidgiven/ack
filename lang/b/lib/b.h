#ifndef B_H
#define B_H

#include <stdlib.h>
#include <stdio.h>

#if EM_PSIZE == 2
#define SHIFT 1
#elif EM_PSIZE == 4
#define SHIFT 2
#elif EM_PSIZE == 8
#define SHIFT 3
#else
#error Unsupported EM_PSIZE
#endif

extern FILE* input_unit;
extern FILE* output_unit;

#define END 4

extern uintptr_t* bmodule_main[];

extern void patch_addresses(uintptr_t** p);

extern void binit(void);

#endif
