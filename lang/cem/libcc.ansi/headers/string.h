/*
 * string.h - string handling
 *
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Header$ */

#if	!defined(__STRING_HEADER__)
#define	__STRING_HEADER__

#if	!defined(NULL)
#define	NULL		0
#endif	/* NULL */

#if	!defined(__TYPE_SIZE__)
#define	__TYPE_SIZE__
typedef unsigned int	size_t;		/* type returned by sizeof */
#endif	/* __TYPE_SIZE__ */

void	*memcpy(void *__s1, const void *__s2, size_t __n);
void	*memmove(void *__s1, const void *__s2, size_t __n);
char	*strcpy(char *__s1, const char *__s2);
char	*strncpy(char *__s1, const char *__s2, size_t __n);
char	*strcat(char *__s1, const char *__s2);
char	*strncat(char *__s1, const char *__s2, size_t __n);
int	memcmp(const void *__s1, const void *__s2, size_t __n);
int	strcmp(const char *__s1, const char *__s2);
int	strcoll(const char *__s1, const char *__s2);
int	strncmp(const char *__s1, const char *__s2, size_t __n);
size_t	strxfrm(char *__s1, const char *__s2, size_t __n);
void	*memchr(const void *__s, int __c, size_t __n);
char	*strchr(const char *__s, int __c);
size_t	strcspn(const char *__s1, const char *__s2);
char	*strpbrk(const char *__s1, const char *__s2);
char	*strrchr(const char *__s, int __c);
size_t	strspn(const char *__s1, const char *__s2);
char	*strstr(const char *__s1, const char *__s2);
char	*strtok(char *__s1, const char *__s2);
void	*memset(void *__s, int __c, size_t __n);
char	*strerror(int __errnum);
size_t	strlen(const char *__s);

#endif	/* __STRING_HEADER__ */
