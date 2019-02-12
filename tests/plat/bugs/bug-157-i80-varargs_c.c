#include <stdio.h>
#include <stdarg.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include "test.h"

static char buf[34];

char * __ultostr(unsigned long val, int radix)
{
   register char *p;
   register int c;

   if( radix > 36 || radix < 2 ) return 0;

   p = buf+sizeof(buf);
   *--p = '\0';

   do
   {
      c = val%radix;
      val/=radix;
      if( c > 9 ) *--p = 'a'-10+c; else *--p = '0'+c;
   }
   while(val);
   return p;
}

char * __ltostr(long val, int radix)
{
   char *p;
   int flg = 0;
   if( val < 0 ) { flg++; val= -val; }
   p = __ultostr(val, radix);
   if(p && flg) *--p = '-';
   return p;
}

void demo(int lval, va_list ap)
{
    char *ptmp = __ltostr((long) ((lval) ?
			    va_arg(ap, long) :
			    va_arg(ap, int)), 10);
    ASSERT(strcmp(ptmp, "35") == 0);
}

void demo2(int lval, va_list ap)
{
    long l;
    char *ptmp;
    
    l = (lval) ?va_arg(ap, long) :
		 va_arg(ap, int);
    ptmp = __ltostr(l, 10);
    ASSERT(strcmp(ptmp, "35") == 0);
}

void doit1a(char *x, ...)
{
    va_list ptr;
    va_start(ptr, x);
    demo(0,ptr);
    va_end(ptr);
}

void doit1b(char *x, ...)
{
    va_list ptr;
    va_start(ptr, x);
    demo(1,ptr);
    va_end(ptr);
}

void doit2a(char *x, ...)
{
    va_list ptr;
    va_start(ptr, x);
    demo2(0,ptr);
    va_end(ptr);
}

void doit2b(char *x, ...)
{
    va_list ptr;
    va_start(ptr, x);
    demo2(1,ptr);
    va_end(ptr);
}

int main(int argc, char *argv[])
{
    doit1a("", 35);
    doit1b("", 35L);
    doit2a("", 35);
    doit2b("", 35L);
    finished();
    return 0;
}
