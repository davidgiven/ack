/*
 * string.h - string handling
 *
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Id$ */

#if	!defined(_STRING_H)
#define	_STRING_H

#define	NULL		((void *)0)

#if	!defined(_SIZE_T)
#define	_SIZE_T
typedef unsigned int	size_t;		/* type returned by sizeof */
#endif	/* _SIZE_T */

void	*memcpy(void *_s1, const void *_s2, size_t _n);
void	*memmove(void *_s1, const void *_s2, size_t _n);
char	*strcpy(char *_s1, const char *_s2);
char	*strncpy(char *_s1, const char *_s2, size_t _n);
char	*strcat(char *_s1, const char *_s2);
char	*strncat(char *_s1, const char *_s2, size_t _n);
int	memcmp(const void *_s1, const void *_s2, size_t _n);
int	strcmp(const char *_s1, const char *_s2);
int	strcoll(const char *_s1, const char *_s2);
int	strncmp(const char *_s1, const char *_s2, size_t _n);
size_t	strxfrm(char *_s1, const char *_s2, size_t _n);
void	*memchr(const void *_s, int _c, size_t _n);
char	*strchr(const char *_s, int _c);
size_t	strcspn(const char *_s1, const char *_s2);
char	*strpbrk(const char *_s1, const char *_s2);
char	*strrchr(const char *_s, int _c);
size_t	strspn(const char *_s1, const char *_s2);
char	*strstr(const char *_s1, const char *_s2);
char	*strtok(char *_s1, const char *_s2);
void	*memset(void *_s, int _c, size_t _n);
char	*strerror(int _errnum);
size_t	strlen(const char *_s);

#endif	/* _STRING_H */
