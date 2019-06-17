#include "b.h"
#include <stdarg.h>
#include <string.h>
#include <limits.h>

extern intptr_t i_main(intptr_t argc, const char* argv[]);

FILE* input_unit;
FILE* output_unit;

static intptr_t i_char(intptr_t s, intptr_t n)
{
    const char* p = (const char*)(s<<SHIFT);
    return p[n];
}

static intptr_t i_lchar(intptr_t s, intptr_t n, intptr_t c)
{
    char* p = (char*)(s<<SHIFT);
    p[n] = c;
	return c;
}

static intptr_t i_getchar(void)
{
    return fgetc(input_unit);
}

static intptr_t i_putchar(intptr_t c)
{
    fputc(c, output_unit);
    return c;
}

static intptr_t i_putstr(intptr_t s)
{
	char* p = (char*)(s<<SHIFT);
	char c;

	while ((c = *p++) != END)
		fputc(c, output_unit);

	return s;
}

static intptr_t i_getstr(intptr_t s)
{
	char* p = (char*)(s<<SHIFT);

	for (;;)
	{
		int c = fgetc(input_unit);
		if ((c == -1) || (c == '\n'))
			break;
		*p++ = c;
	}

	*p++ = END;
	return s;
}
	
static void i_flush(void)
{
	fflush(output_unit);
}

static int tochar(int n)
{
	if (n <= 9)
		return n + '0';
	return n - 10 + 'a';
}

static void putnum(intptr_t value, int base)
{
	int i;
	char s[32];

	if (value < 0)
	{
		fputc('-', output_unit);
		value = -value;
	}

	i = 0;
	do
		s[i++] = tochar(value % base);
	while ((value /= base) > 0);

	do
		fputc(s[--i], output_unit);
	while (i > 0);
}

static void i_printf(intptr_t s, ...)
{
	char* p = (char*)(s<<SHIFT);
	char c;

	va_list ap;
	va_start(ap, s);

	while ((c = *p++) != END)
	{
		switch (c)
		{
			case '%':
			{
				intptr_t ss = va_arg(ap, intptr_t);

				switch ((c = *p++))
				{
					case 's':
						i_putstr(ss);
						break;

					case 'd':
						putnum(ss, 10);
						break;

					case 'o':
						putnum(ss, 8);
						break;

					case 'x':
						putnum(ss, 16);
						break;

					case 'c':
						fputc(ss, output_unit);
						break;

					default:
						fputc('?', output_unit);
						break;
				}
				break;
			}
				
			default:
				fputc(c, output_unit);
				break;
		}
	}

	va_end(ap);
}

uintptr_t b_char = (uintptr_t)i_char;
uintptr_t b_lchar = (uintptr_t)i_lchar;
uintptr_t b_getchar = (uintptr_t)i_getchar;
uintptr_t b_putchar = (uintptr_t)i_putchar;
uintptr_t b_putstr = (uintptr_t)i_putstr;
uintptr_t b_getstr = (uintptr_t)i_getstr;
uintptr_t b_flush = (uintptr_t)i_flush;
uintptr_t b_printf = (uintptr_t)i_printf;

static uintptr_t* bmodule_stdlib[] =
{
    &b_char,
    &b_lchar,
    &b_getchar,
    &b_putchar,
	&b_putstr,
	&b_getstr,
	&b_flush,
	&b_printf,
    0
};

void patch_addresses(uintptr_t** p)
{
    while (*p)
    {
        uintptr_t* q = *p++;
		if (*q & MASK)
			abort();
        *q >>= SHIFT;
    }
}

int main(int argc, const char* argv[])
{
    patch_addresses(bmodule_stdlib);
	binit();
    input_unit = stdin;
    output_unit = stdout;
    return i_main(argc, NULL);
}
