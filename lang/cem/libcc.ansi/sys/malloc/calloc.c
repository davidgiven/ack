#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>

#if ACKCONF_WANT_MALLOC

void* calloc(size_t nmemb, size_t size)
{
	size_t bytes = nmemb * size;
	void* ptr;

	/* Test for overflow.
     * See http://stackoverflow.com/questions/1815367/multiplication-of-large-numbers-how-to-catch-overflow
     */

	if ((nmemb == 0) || (size == 0) || (nmemb > (SIZE_MAX / size)))
		return NULL;

	ptr = malloc(bytes);
	if (!ptr)
		return NULL;

	memset(ptr, 0, bytes);
	return ptr;
}

#endif
