#include "b.h"

extern uintptr_t* bsymb_patch_table[];
extern intptr_t i_main(intptr_t argc, const char* argv[]);

FILE* output_unit;

static intptr_t i_char(intptr_t s, intptr_t n)
{
    const char* p = (const char*)(s<<SHIFT);
    return p[n];
}

static void i_lchar(intptr_t s, intptr_t n, intptr_t c)
{
    char* p = (char*)(s<<SHIFT);
    p[n] = c;
}

static intptr_t i_getchar(void)
{
    return fgetc(output_unit);
}

static void i_putchar(intptr_t c)
{
    fputc(c, output_unit);
}

uintptr_t b_char = (uintptr_t)i_char;
uintptr_t b_lchar = (uintptr_t)i_lchar;
uintptr_t b_getchar = (uintptr_t)i_getchar;
uintptr_t b_putchar = (uintptr_t)i_putchar;

static uintptr_t* lib_patch_table[] =
{
    &b_char,
    &b_lchar,
    &b_getchar,
    &b_putchar,
    0
};

static void patch_addresses(uintptr_t** p)
{
    while (*p)
    {
        uintptr_t* q = *p++;
        *q >>= SHIFT;
    }
}

int main(int argc, const char* argv[])
{
    patch_addresses(bsymb_patch_table);
    patch_addresses(lib_patch_table);
    output_unit = stdout;
    return i_main(argc, NULL);
}