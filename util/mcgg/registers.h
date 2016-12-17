#ifndef REGISTERS_H
#define REGISTERS_H

#include "array.h"

struct reg
{
	const char* name;          /* friendly register name */
	int number;                /* identifying number */
	uint32_t attrs;            /* bitfield of register attributes */
	struct stringlist* names;  /* register names */
	unsigned int* bitmap;      /* hardware register bitmap */
	ARRAYOF(struct reg) aliases; /* registers that this one aliases */
};

struct regattr
{
	const char* name;      /* class name */
	int number;            /* identifying number */
	unsigned int* bitmap;  /* hardware register bitmap */
};

extern struct reg* makereg(const char* name);
extern void setregnames(struct reg* reg, struct stringlist* names);
extern void addregattr(struct reg* reg, const char* regattr);
extern void addregaliases(struct reg* reg, struct stringlist* aliases);
extern struct regattr* getregattr(const char* name);

extern void analyse_registers(void);

extern void emitregisterattrs(void);
extern void emitregisters(void);

extern struct regattr* findregattr(const char* name);

#endif

