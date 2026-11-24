/*
 * flt_add, flt_sub, flt_mul and flt_div followed by flt_rnd must give what
 * IEEE arithmetic gives: checked against the host's double and float on
 * random operands, subnormal results and exact ties included.
 *
 *	ieeetest [count]
 */

#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "flt_arith.h"

static uint64_t state = 0x2545F4914F6CDD1DULL;

static uint64_t rnd(void)
{
	/* xorshift64*: the same numbers on every host */
	state ^= state >> 12;
	state ^= state << 25;
	state ^= state >> 27;
	return state * 0x2545F4914F6CDD1DULL;
}

static void d2flt(double d, flt_arith* e)
{
	int exp;
	double f = frexp(fabs(d), &exp); /* d = f * 2^exp, f in [0.5, 1) */
	uint64_t m = (uint64_t)ldexp(f, 64);

	e->flt_sign = d < 0;
	e->flt_exp = exp - 1;
	e->flt_mantissa.flt_h_32 = (uint32_t)(m >> 32);
	e->flt_mantissa.flt_l_32 = (uint32_t)m;
	if (d == 0)
		e->flt_exp = e->flt_sign = 0;
}

static double flt2d(flt_arith* e)
{
	uint64_t m = ((uint64_t)e->flt_mantissa.flt_h_32 << 32) | e->flt_mantissa.flt_l_32;
	double d = ldexp((double)m, e->flt_exp - 63);

	return e->flt_sign ? -d : d;
}

/* A random double with an exponent in [-emax, emax]; bits mantissa bits. */
static double randnum(int emax, int bits)
{
	uint64_t m = rnd() >> (64 - bits) | (uint64_t)1 << (bits - 1);
	double d = ldexp((double)m, (int)(rnd() % (2 * emax + 1)) - emax - bits + 1);

	return (rnd() & 1) ? -d : d;
}

static long checked, failures;

static void check(int isfloat, char op, double a, double b)
{
	volatile double dwant;
	volatile float fwant;
	double want, got;
	flt_arith e1, e2, r;

	if (isfloat)
	{
		volatile float fa = (float)a, fb = (float)b;

		a = fa;
		b = fb;
		switch (op)
		{
			case '+': fwant = fa + fb; break;
			case '-': fwant = fa - fb; break;
			case '*': fwant = fa * fb; break;
			default: fwant = fa / fb; break;
		}
		want = fwant;
	}
	else
	{
		switch (op)
		{
			case '+': dwant = a + b; break;
			case '-': dwant = a - b; break;
			case '*': dwant = a * b; break;
			default: dwant = a / b; break;
		}
		want = dwant;
	}
	/* flt_arith has a wider range and no infinity */
	if (!isfinite(a) || !isfinite(b) || !isfinite(want) || (op == '/' && b == 0))
		return;

	d2flt(a, &e1);
	d2flt(b, &e2);
	switch (op)
	{
		case '+': flt_add(&e1, &e2, &r); break;
		case '-': flt_sub(&e1, &e2, &r); break;
		case '*': flt_mul(&e1, &e2, &r); break;
		default: flt_div(&e1, &e2, &r); break;
	}
	if (isfloat)
		flt_rnd(&r, 24, -126);
	else
		flt_rnd(&r, 53, -1022);
	got = flt2d(&r);
	checked++;
	/* flt_arith has no negative zero */
	if (want == 0 ? got != 0 : got != want)
	{
		if (failures++ < 10)
			printf("%s %a %c %a: want %a, got %a\n", isfloat ? "float" : "double", a, op, b,
			    want, got);
	}
}

int main(int argc, char* argv[])
{
	long count = argc > 1 ? atol(argv[1]) : 200000;
	static const char ops[] = "+-*/";
	long i;
	int k, isfloat;

	for (i = 0; i < count; i++)
	{
		for (isfloat = 0; isfloat <= 1; isfloat++)
		{
			int emax = isfloat ? 127 : 1023;
			int bits = isfloat ? 24 : 53;
			double a, b;

			for (k = 0; k < 4; k++)
			{
				switch (rnd() % 5)
				{
					case 0: /* any magnitudes */
						a = randnum(emax, bits);
						b = randnum(emax, bits);
						break;
					case 1: /* close magnitudes: cancellation, carries */
						a = randnum(4, bits);
						b = randnum(4, bits);
						break;
					case 2: /* short mantissas: exact results and ties */
						a = randnum(emax / 8, 1 + (int)(rnd() % 12));
						b = randnum(emax / 8, 1 + (int)(rnd() % 12));
						break;
					case 3: /* a tiny, b near 1: subnormal products */
						a = ldexp(randnum(0, bits), -(emax - 1) + (int)(rnd() % 8));
						b = ldexp(randnum(0, bits), -(int)(rnd() % (bits + 4)));
						break;
					default: /* both tiny: subnormal sums */
						a = ldexp(randnum(0, bits), -(emax - 1) + (int)(rnd() % 4));
						b = ldexp(randnum(0, bits), -(emax - 1) + (int)(rnd() % 4));
						break;
				}
				check(isfloat, ops[k], a, b);
			}
		}
	}
	if (failures)
	{
		printf("@@FAIL flt_arith: %ld of %ld results not as in IEEE arithmetic\n",
		    failures, checked);
		return 1;
	}
	printf("flt_arith: %ld results as in IEEE arithmetic\n", checked);
	return 0;
}
