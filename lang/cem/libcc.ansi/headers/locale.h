/*
 * locale.h - localization
 */
/* $Header$ */

#ifndef	_LOCALE_HEADER_
#define	_LOCALE_HEADER_

struct lconv {
	char	*decimal_point;		/* "." */
	char	*thousands_sep;		/* "" */
	char	*grouping;		/* "" */
	char	*int_curr_symbol;	/* "" */
	char	*currency_symbol;	/* "" */
	char	*mon_decimal_point;	/* "" */
	char	*mon_thousands_sep;	/* "" */
	char	*mon_grouping;		/* "" */
	char	*positive_sign;		/* "" */
	char	*negative_sign;		/* "" */
	char	frac_digits;		/* CHAR_MAX */
	char	p_cs_precedes;		/* CHAR_MAX */
	char	p_sep_by_space;		/* CHAR_MAX */
	char	n_cs_precedes;		/* CHAR_MAX */
	char	n_sep_by_space;		/* CHAR_MAX */
	char	p_sign_posn;		/* CHAR_MAX */
	char	n_sign_posn;		/* CHAR_MAX */
};

#ifndef	NULL
#define	NULL		0
#endif	/* NULL */

#define	LC_ALL		1
#define	LC_COLLATE	2
#define	LC_CTYPE	3
#define	LC_MONETARY	4
#define	LC_NUMERIC	5
#define	LC_TIME		6


char		*setlocale(int category, const char *locale);
struct	lconv	*localeconv(void);

#endif	/* _LOCALE_HEADER_ */
