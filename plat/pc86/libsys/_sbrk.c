/* There should be a header for brk and sbrk, but there isn't. */

#include <stdlib.h>
#include <errno.h>
/* #include <unistd.h> */

extern char _end[];
static char* brkpointer = _end;

static void prints(char* s)
{
	for (;;)
	{
		char c = *s++;
		if (!c)
			break;
		write(0, &c, 1);
	}
}

static void printc(unsigned int n)
{
	char c;
	
	n &= 0xF;
	if (n < 10)
		c = n + '0';
	else
		c = n + 'A' - 10;
	
	write(0, &c, 1);
}

static void printh(unsigned int n)
{
	printc(n>>12);
	printc(n>>8);
	printc(n>>4);
	printc(n);
}
 
static void waitforkey(void)
{
	char c;
	read(1, &c, 1);
}

int _brk(char* newend)
{
	char dummy;
	
	/* Ensure that newend is reasonable. */
	
	if ((newend < _end) || (newend > (&dummy - 256)))
	{
		prints("[brk to ");
		printh((unsigned int) newend);
		prints(" failed]\n\r");
		waitforkey();
		errno = ENOMEM;
		return -1;
	}
	
	prints("[brk to ");
	printh((unsigned int) newend);
	prints("]\n\r");
	waitforkey();
	brkpointer = newend;
	return 0;
}

char* _sbrk(int delta)
{
	char* oldpointer = brkpointer;
	prints("[sbrk delta ");
	printh((unsigned int) delta);
	prints(" from ");
	printh((unsigned int) oldpointer);
	prints("]\n\r");
	printh((unsigned int) brkpointer);
	prints(" ");
	printh((unsigned int) _end);
	if (_brk(oldpointer + delta) == -1)
		return (char*)-1;
	
	return oldpointer;
}
