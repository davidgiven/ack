#ifndef TEST_H
#define TEST_H

#include <unistd.h>
#include <stdint.h>

extern void finished(void);
extern void writehex(unsigned int code);
extern void fail(unsigned int code);

#define ASSERT(condition) \
    do { if (!(condition)) fail(__LINE__); } while(0)

#endif
