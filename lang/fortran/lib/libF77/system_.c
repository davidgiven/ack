/* f77 interface to system routine */

#include "f2c.h"

 integer
system_(s, n)
char *s;
long int n;
{
char buff[1000];
char *bp, *blast;

blast = buff + (n < 1000 ? n : 1000);

for(bp = buff ; bp<blast && *s!='\0' ; )
	*bp++ = *s++;
*bp = '\0';
return system(buff);
}
