/* $Header$ */
/* STRING-ROUTINE DEFINITIONS */

#define stdin 0
#define stdout 1
#define stderr 2

#define itos(n)	int_str((long)(n), 10)

char *sprintf();	/* string.h	*/
char *int_str();	/* string.h	*/

char *strcpy(), *strcat(), *rindex();
