#ifndef BUFFER_H
#define BUFFER_H

struct buffer
{
	char* ptr;
	size_t len;
	size_t size;
};

extern void buffer_appendf(struct buffer* buffer, const char* fmt, ...);
extern void buffer_appendfv(struct buffer* buffer, const char* fmt, va_list ap);
extern void buffer_clear(struct buffer* buffer);

#endif

