#include <limits.h>
#include "test.h"

/* Constants in globals to defeat constant folding. */
int one = 1;
int zero = 0;
int minusone = -1;

/* Bypasses the CRT, so there's no stdio. */
void _m_a_i_n(void)
{
    ASSERT((one     <<zero) == 1);
    ASSERT((one     <<one)  == 2);
    ASSERT((minusone<<zero) == -1);
    ASSERT((minusone<<one)  == -2);

    ASSERT(((unsigned int)one     <<(unsigned int)zero) == 1);
    ASSERT(((unsigned int)one     <<(unsigned int)one)  == 2);
    ASSERT(((unsigned int)minusone<<(unsigned int)zero) == -1);
    ASSERT(((unsigned int)minusone<<(unsigned int)one)  == -2);

    ASSERT((one     >>zero) == 1);
    ASSERT((one     >>one)  == 0);
    ASSERT((minusone>>zero) == -1);
    ASSERT((minusone>>one)  == -1);

    ASSERT(((unsigned int)one     >>(unsigned int)zero) == 1);
    ASSERT(((unsigned int)one     >>(unsigned int)one)  == 0);
    ASSERT(((unsigned int)minusone>>(unsigned int)zero) == UINT_MAX);
    ASSERT(((unsigned int)minusone>>(unsigned int)one)  == (UINT_MAX>>1));

    ASSERT((one     <<0) == 1);
    ASSERT((one     <<1)  == 2);
    ASSERT((minusone<<0) == -1);
    ASSERT((minusone<<1)  == -2);

    ASSERT(((unsigned int)one     <<(unsigned int)0) == 1);
    ASSERT(((unsigned int)one     <<(unsigned int)1)  == 2);
    ASSERT(((unsigned int)minusone<<(unsigned int)0) == -1);
    ASSERT(((unsigned int)minusone<<(unsigned int)1)  == -2);

    ASSERT((one     >>0) == 1);
    ASSERT((one     >>1)  == 0);
    ASSERT((minusone>>0) == -1);
    ASSERT((minusone>>1)  == -1);

    ASSERT(((unsigned int)one     >>(unsigned int)0) == 1);
    ASSERT(((unsigned int)one     >>(unsigned int)1)  == 0);
    ASSERT(((unsigned int)minusone>>(unsigned int)0) == UINT_MAX);
    ASSERT(((unsigned int)minusone>>(unsigned int)1)  == (UINT_MAX>>1));

    finished();
}
