#include "test.h"

/* Constants in globals to defeat constant folding. */
long two = 2;
long one = 1;
long zero = 0;
long minusone = -1;

/* Bypasses the CRT, so there's no stdio. */
void _m_a_i_n(void)
{
	/* Adds */

    ASSERT((two + one)      == 3);
    ASSERT((two + minusone) == 1);

    ASSERT((two +  1) == 3);
    ASSERT((two + -1) == 1);

    ASSERT((1   + two) == 3);
    ASSERT((-1  + two) == 1);

    ASSERT(((unsigned long)two + (unsigned long)one)      == 3);
    ASSERT(((unsigned long)two + (unsigned long)minusone) == 1);

    ASSERT(((unsigned long)two + (unsigned long) 1) == 3);
    ASSERT(((unsigned long)two + (unsigned long)-1) == 1);

    ASSERT(((unsigned long)1   + (unsigned long)two) == 3);
    ASSERT(((unsigned long)-1  + (unsigned long)two) == 1);

    ASSERT(((unsigned long)two + (unsigned long)256) == 258);

	/* Simple operators */

	ASSERT(-two == -2);
	
    finished();
}
