/*  Copyright (c) 2019. See the file License in
 *  the root directory for more information.
 *
 *  Contains path related utilities.
 */
#include <string.h>


void sys_basename(char *str, register char *dst)
{
	register char *p1 = str;
	register char *p2 = p1;
	register char *end;
	register char *start;

	int len = strlen(str);
	/* Point to the end of the string. */
	p1 = p1 + len - 1;
	end = p1;

	while ((*p1 == '/') || (*p1 == '\\'))
	{
		if (p1 == str)
		{
			dst[0] = *p1;
			dst[1] = '\0';
			return;
		}
		p1--;
	}
	/* Only a volume specification */
	if (*p1 == ':')
	{
		strcpy(dst,str);
		return;
	}
	/* Do a reverse search. */
	p2 = p1;
	len = 0;
	while (p2 != str)
	{
		if ((*p1 == '/') || (*p1 == '\\') || (*p1 == ':'))
		{
			strncpy(dst,p2,len);
			dst[len] = '\0';
			return;
		}
		p2 = p1;
		len++;
		p1--;
	}
   /* Only a pathname */
   strcpy(dst,str);
}



