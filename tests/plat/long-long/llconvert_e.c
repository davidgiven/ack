#include <unistd.h>
#include "test.h"

char coal = 12;
short stop = 3456;
int erest = 7890;
long way = 123456789L;

signed char ter = -1;
short sale = -9876;
int ern = -5432;
long itude = -1000000L;

unsigned char ming = 200;
unsigned short age = 40000U;
unsigned int othe = 50000U;
unsigned long shore = 3000000000UL;

long long ago;
unsigned long long ull;

/*
 * BAR may modify global variables (though it really doesn't).  The
 * compiler should not assume that "ago" has the same value before and
 * after BAR, but should generate code to read "ago" again.
 */
#define BAR write(1, "", 0)

void _m_a_i_n(void) {
	ago = coal;
	BAR;
	ASSERT(ago == coal);
	ASSERT(ago == 12LL);

	ago = stop;
	BAR;
	ASSERT(ago == stop);
	ASSERT(ago == 3456LL);

	ago = erest;
	BAR;
	ASSERT(ago == erest);
	ASSERT(ago == 7890LL);

	ago = way;
	BAR;
	ASSERT(ago == way);
	ASSERT(ago == 123456789LL);

	ull = coal;
	BAR;
	ASSERT(ull == coal);
	ASSERT(ull == 12ULL);

	ull = stop;
	BAR;
	ASSERT(ull == stop);
	ASSERT(ull == 3456ULL);

	ull = erest;
	BAR;
	ASSERT(ull == erest);
	ASSERT(ull == 7890ULL);

	ull = way;
	BAR;
	ASSERT(ull == way);
	ASSERT(ull == 123456789ULL);

	ago = ter;
	BAR;
	ASSERT(ago == ter);
	ASSERT(ago == -1LL);

	ago = sale;
	BAR;
	ASSERT(ago == sale);
	ASSERT(ago == -9876LL);

	ago = ern;
	BAR;
	ASSERT(ago == ern);
	ASSERT(ago == -5432LL);

	ago = itude;
	BAR;
	ASSERT(ago == itude);
	ASSERT(ago == -1000000LL);

	ago = ming;
	BAR;
	ASSERT(ago == ming);
	ASSERT(ago == 200LL);

	ago = age;
	BAR;
	ASSERT(ago == age);
	ASSERT(ago == 40000LL);

	ago = othe;
	BAR;
	ASSERT(ago == othe);
	ASSERT(ago == 50000LL);

	ago = shore;
	BAR;
	ASSERT(ago == shore);
	ASSERT(ago == 3000000000LL);

	ull = ming;
	BAR;
	ASSERT(ull == ming);
	ASSERT(ull == 200ULL);

	ull = age;
	BAR;
	ASSERT(ull == age);
	ASSERT(ull == 40000ULL);

	ull = othe;
	BAR;
	ASSERT(ull == othe);
	ASSERT(ull == 50000ULL);

	ull = shore;
	BAR;
	ASSERT(ull == shore);
	ASSERT(ull == 3000000000ULL);

	ago = 95;
	BAR;
	ter = ago;
	sale = ago;
	ern = ago;
	itude = ago;
	ming = ago;
	age = ago;
	othe = ago;
	shore = ago;
	BAR;
	ASSERT(ter == 95);
	ASSERT(sale == 95);
	ASSERT(ern == 95);
	ASSERT(itude == 95L);
	ASSERT(ming == 95);
	ASSERT(age == 95U);
	ASSERT(othe == 95U);
	ASSERT(shore == 95UL);

	ago = -59;
	BAR;
	ter = ago;
	sale = ago;
	ern = ago;
	itude = ago;
	BAR;
	ASSERT(ter == -59);
	ASSERT(sale == -59);
	ASSERT(ern == -59);
	ASSERT(itude == -59L);

	ull = 42;
	BAR;
	ter = ull;
	sale = ull;
	ern = ull;
	itude = ull;
	ming = ull;
	age = ull;
	othe = ull;
	shore = ull;
	BAR;
	ASSERT(ter == 42);
	ASSERT(sale == 42);
	ASSERT(ern == 42);
	ASSERT(itude == 42L);
	ASSERT(ming == 42);
	ASSERT(age == 42U);
	ASSERT(othe == 42U);
	ASSERT(shore == 42UL);

	finished();
}
