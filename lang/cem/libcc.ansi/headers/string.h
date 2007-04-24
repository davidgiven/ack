/*
 * string.h - string handling
 *
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Id$ */

#ifndef _STRING_H
#define	_STRING_H

#include <stddef.h>

extern void	*memcpy(void *_s1, const void *_s2, size_t _n);
extern void	*memmove(void *_s1, const void *_s2, size_t _n);
extern char	*strcpy(char *_s1, const char *_s2);
extern char	*strncpy(char *_s1, const char *_s2, size_t _n);
extern char	*strcat(char *_s1, const char *_s2);
extern char	*strncat(char *_s1, const char *_s2, size_t _n);
extern int	memcmp(const void *_s1, const void *_s2, size_t _n);
extern int	strcmp(const char *_s1, const char *_s2);
extern int	strcoll(const char *_s1, const char *_s2);
extern int	strncmp(const char *_s1, const char *_s2, size_t _n);
extern size_t	strxfrm(char *_s1, const char *_s2, size_t _n);
extern void	*memchr(const void *_s, int _c, size_t _n);
extern char	*strchr(const char *_s, int _c);
extern size_t	strcspn(const char *_s1, const char *_s2);
extern char	*strpbrk(const char *_s1, const char *_s2);
extern char	*strrchr(const char *_s, int _c);
extern size_t	strspn(const char *_s1, const char *_s2);
extern char	*strstr(const char *_s1, const char *_s2);
extern char	*strtok(char *_s1, const char *_s2);
extern void	*memset(void *_s, int _c, size_t _n);
extern char	*strerror(int _errnum);
extern size_t	strlen(const char *_s);

#endif
