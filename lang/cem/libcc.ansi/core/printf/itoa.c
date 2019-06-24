#include <stdio.h>

void itoa(long value, char* buffer, int radix)
{
	if (value < 0)
	{
		*buffer++ = '-';
		value = -value;
	}
	buffer = _i_compute(value, 10, buffer, 0);
	*buffer++ = '\0';
}

