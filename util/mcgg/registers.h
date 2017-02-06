#ifndef REGISTERS_H
#define REGISTERS_H

#include "array.h"
#include "set.h"

struct reg
{
	const char* name;          /* friendly register name */
	int number;                /* identifying number */
	uint32_t attrs;            /* bitfield of register attributes */
	struct set classes;        /* register classes this register belongs to */
	struct stringlist* names;  /* register names */
	unsigned int* bitmap;      /* hardware register bitmap */
	ARRAYOF(struct reg) uses;  /* registers that this one uses */
	int weight;                /* weight of this register */
};

struct regattr
{
	const char* name;      /* class name */
	int number;            /* identifying number */
	unsigned int* bitmap;  /* hardware register bitmap */
	int weight;            /* maximum weight of all registers in this class */
};

extern int real_register_count;

extern struct reg* makereg(const char* name);
extern void setregnames(struct reg* reg, struct stringlist* names);
extern void addregattr(struct reg* reg, const char* regattr);
extern void addregusage(struct reg* reg, struct stringlist* uses);
extern struct regattr* getregattr(const char* name);

extern void analyse_registers(void);

extern void emitregisterattrs(void);
extern void emitregisters(void);

extern struct regattr* findregattr(const char* name);

#endif

