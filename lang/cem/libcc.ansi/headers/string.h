/*
 * string.h - string handling
 *
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Header$ */

#ifndef	_STRING_HEADER_
#define	_STRING_HEADER_

#ifndef	NULL
#define	NULL		0
#endif	/* NULL */

#ifndef	_TYPE_SIZE_
#define	_TYPE_SIZE_
typedef unsigned int	size_t;		/* type returned by sizeof */
#endif	/* _TYPE_SIZE_ */

#ifdef	__STDC__
void	*memcpy(void *s1, const void *s2, size_t n);
void	*memmove(void *s1, const void *s2, size_t n);
char	*strcpy(char *s1, const char *s2);
char	*strncpy(char *s1, const char *s2, size_t n);
char	*strcat(char *s1, const char *s2);
char	*strncat(char *s1, const char *s2, size_t n);
int	memcmp(const void *s1, const void *s2, size_t n);
int	strcmp(const char *s1, const char *s2);
int	strcoll(const char *s1, const char *s2);
int	strncmp(const char *s1, const char *s2, size_t n);
size_t	strxfrm(char *s1, const char *s2, size_t n);
void	*memchr(const void *s, int c, size_t n);
char	*strchr(const char *s, int c);
size_t	strcspn(const char *s1, const char *s2);
char	*strpbrk(const char *s1, const char *s2);
char	*strrchr(const char *s, int c);
size_t	strspn(const char *s1, const char *s2);
char	*strstr(const char *s1, const char *s2);
char	*strtok(char *s1, const char *s2);
void	*memset(void *s, int c, size_t n);
char	*strerror(int errnum);
size_t	strlen(const char *s);
#endif	/* __STDC__ */

#endif	/* _STRING_HEADER_ */
