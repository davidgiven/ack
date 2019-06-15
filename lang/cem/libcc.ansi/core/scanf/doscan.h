#ifndef DOSCAN_H
#define DOSCAN_H

#define	FL_LJUST	0x0001		/* left-justify field */
#define	FL_SIGN		0x0002		/* sign in signed conversions */
#define	FL_SPACE	0x0004		/* space in signed conversions */
#define	FL_ALT		0x0008		/* alternate form */
#define	FL_ZEROFILL	0x0010		/* fill with zero's */
#define	FL_SHORT	0x0020		/* optional h */
#define	FL_LONG		0x0040		/* optional l */
#define	FL_LONGDOUBLE	0x0080		/* optional L */
#define	FL_WIDTHSPEC	0x0100		/* field width is specified */
#define	FL_PRECSPEC	0x0200		/* precision is specified */
#define FL_SIGNEDCONV	0x0400		/* may contain a sign */
#define	FL_NOASSIGN	0x0800		/* do not assign (in scanf) */
#define	FL_NOMORE	0x1000		/* all flags collected */

extern int (*_doscan_get)(void);
extern void (*_doscan_unget)(int c);

extern int _doscan(const char* format, va_list ap);

#endif
