#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdarg.h>
#include <string.h>
#include <assert.h>
#include "buffer.h"

#define MIN_SIZE 64

void buffer_appendfv(struct buffer* buffer, const char* fmt, va_list ap)
{
	int n;
	va_list ap1;

	va_copy(ap1, ap);
	n = buffer->len + vsnprintf(NULL, 0, fmt, ap1) + 1;
	va_end(ap1);

	if (n > buffer->size)
	{
		buffer->size *= 2;
		if (buffer->size < n)
			buffer->size = n * 2;
		if (buffer->size < MIN_SIZE)
			buffer->size = MIN_SIZE;
		buffer->ptr = realloc(buffer->ptr, buffer->size);
	}

	vsprintf(buffer->ptr + buffer->len, fmt, ap);

	buffer->len = n - 1; /* remember the \0 at the end */
}

void buffer_appendf(struct buffer* buffer, const char* fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	buffer_appendfv(buffer, fmt, ap);
	va_end(ap);
}

void buffer_clear(struct buffer* buffer)
{
	if (!buffer->ptr)
	{
		buffer->ptr = malloc(MIN_SIZE);
		buffer->size = MIN_SIZE;
	}

	buffer->ptr[0] = '\0';
	buffer->len = 0;
}

