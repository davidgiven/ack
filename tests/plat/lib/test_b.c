#include <stdint.h>
#include "test.h"

extern void patch_addresses(uintptr_t* module);
extern uintptr_t* bmodule_main;

static void i_writehex(intptr_t code)
{
	writehex(code);
}

static void i_fail(intptr_t code)
{
	fail(code);
}

uintptr_t b_finished = (uintptr_t)&finished;
uintptr_t b_writehex = (uintptr_t)&i_writehex;
uintptr_t b_fail = (uintptr_t)&i_fail;

static uintptr_t* bmodule_test[] =
{
    &b_finished,
    &b_writehex,
    &b_fail,
	0
};

void binit(void)
{
	patch_addresses(&bmodule_test);
	patch_addresses(&bmodule_main);
}

