/* The <string.h> header contains prototypes for the string handling 
 * functions.
 */

#ifndef _STRING_H
#define _STRING_H

#define NULL    ((void *)0)

#ifndef _SIZE_T
#define _SIZE_T
typedef unsigned int size_t;	/* type returned by sizeof */
#endif /*_SIZE_T */

/* Function Prototypes. */
#ifndef _ANSI_H
#include <ansi.h>
#endif

_PROTOTYPE( void *memcpy, (void *_s1, const void *_s2, size_t _n)	);
_PROTOTYPE( void *memmove, (void *_s1, const void *_s2, size_t _n)	);
_PROTOTYPE( char *strcpy, (char *_s1, const char *_s2)			);
_PROTOTYPE( char *strncpy, (char *_s1, const char *_s2, size_t _n)	);
_PROTOTYPE( char *strcat, (char *_s1, const char *_s2)			);
_PROTOTYPE( char *strncat, (char *_s1, const char *_s2, size_t _n)	);
_PROTOTYPE( int memcmp, (const void *_s1, const void *_s2, size_t _n)	);
_PROTOTYPE( int strcmp, (const char *_s1, const char *_s2)		);
_PROTOTYPE( int strcoll, (const char *_s1, const char *_s2)		);
_PROTOTYPE( int strncmp, (const char *_s1, const char *_s2, size_t _n)	);
_PROTOTYPE( size_t strxfrm, (char *_s1, const char *_s2, size_t _n)	);
_PROTOTYPE( void *memchr, (const void *_s, int _c, size_t _n)		);
_PROTOTYPE( char *strchr, (const char *_s, int _c)			);
_PROTOTYPE( size_t strcspn, (const char *_s1, const char *_s2)		);
_PROTOTYPE( char *strpbrk, (const char *_s1, const char *_s2)		);
_PROTOTYPE( char *strrchr, (const char *_s, int _c)			);
_PROTOTYPE( size_t strspn, (const char *_s1, const char *_s2)		);
_PROTOTYPE( char *strstr, (const char *_s1, const char *_s2)		);
_PROTOTYPE( char *strtok, (char *_s1, const char *_s2)			);
_PROTOTYPE( void *memset, (void *_s, int _c, size_t _n)			);
_PROTOTYPE( char *strerror, ( int _errnum)				);
_PROTOTYPE( size_t strlen, (const char *_s)				);

#ifdef _MINIX
/* For backward compatibility. */
_PROTOTYPE( char *index, (const char *_s, int _charwanted)		);
_PROTOTYPE( char *rindex, (const char *_s, int _charwanted)		);
_PROTOTYPE( void bcopy, (const char *_src, char *_dst, int _length)	);
_PROTOTYPE( int bcmp, (const char *_s1, const char *_s2, int _length)	);
_PROTOTYPE( void bzero, (char *_dst, int _length)			);
_PROTOTYPE( void *memccpy, (char *_dst, const char *_src, int _ucharstop,
						    size_t _size)	);

#endif

#endif /* _STRING_H */
