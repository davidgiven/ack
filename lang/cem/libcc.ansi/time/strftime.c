/*
 * strftime - convert a structure to a string, controlled by an argument
 */
/* $Header$ */

#include	<time.h>
#include	"loc_incl.h"

/* The width can be negative in both s_prnt() as in u_prnt(). This
 * indicates that as many characters as needed should be printed.
 */
static char *
s_prnt(char *s, size_t maxsize, const char *str, int width)
{
	while (width > 0 || (width < 0 && *str)) {
		if (!maxsize) break;
		*s++ = *str++;
		maxsize--;
		width--;
	}
	return s;
}

static char *
u_prnt(char *s, size_t maxsize, unsigned val, int width)
{
	int c;

	c = val % 10;
	val = val / 10;
	if (--width > 0 || (width < 0 && val != 0))
		s = u_prnt(s, (maxsize ? maxsize - 1 : 0), val, width);
	if (maxsize) *s++ = c + '0';
	return s;
}

size_t
strftime(char *s, size_t maxsize,
		const char *format, const struct tm *timeptr)
{
	size_t n;
	char *firsts, *olds;

	if (!format) return 0;

	firsts = s;
	while (maxsize && *format) {
		if (maxsize && *format && *format != '%') {
			*s++ = *format++;
			maxsize--;
			continue;
		}
		if (*format++ != '%') break;	/* maxsize == 0 || !*format */

		olds = s;
		switch (*format++) {
		case 'a':
			s = s_prnt(s, maxsize,
					_days[timeptr->tm_wday], ABB_LEN);
			maxsize -= s - olds;
			break;
		case 'A':
			s = s_prnt(s, maxsize, _days[timeptr->tm_wday], -1);
			maxsize -= s - olds;
			break;
		case 'b':
			s = s_prnt(s, maxsize,
					_months[timeptr->tm_mon], ABB_LEN);
			maxsize -= s - olds;
			break;
		case 'B':
			s = s_prnt(s, maxsize, _months[timeptr->tm_mon], -1);
			maxsize -= s - olds;
			break;
		case 'c':
			n = strftime(s, maxsize,
					"%a %b %d %H:%M:%S %Y", timeptr);
			maxsize -= n;
			s += n;
			break;
		case 'd':
			s = u_prnt(s, maxsize, timeptr->tm_mday, 2);
			maxsize -= s - olds;
			break;
		case 'H':
			s = u_prnt(s, maxsize, timeptr->tm_hour, 2);
			maxsize -= s - olds;
			break;
		case 'I':
			s = u_prnt(s, maxsize,
					(timeptr->tm_hour + 11) % 12 + 1, 2);
			maxsize -= s - olds;
			break;
		case 'j':
			s = u_prnt(s, maxsize, timeptr->tm_yday + 1, 3);
			maxsize -= s - olds;
			break;
		case 'm':
			s = u_prnt(s, maxsize, timeptr->tm_mon + 1, 2);
			maxsize -= s - olds;
			break;
		case 'M':
			s = u_prnt(s, maxsize, timeptr->tm_min, 2);
			maxsize -= s - olds;
			break;
		case 'p':
			s = s_prnt(s, maxsize,
				    (timeptr->tm_hour < 12) ? "AM" : "PM", 2);
			maxsize -= s - olds;
			break;
		case 'S':
			s = u_prnt(s, maxsize, timeptr->tm_sec, 2);
			maxsize -= s - olds;
			break;
		case 'U':
			s = u_prnt(s, maxsize,		/* ??? */
			    (timeptr->tm_yday + 7 - timeptr->tm_wday) / 7, 2);
			maxsize -= s - olds;
			break;
		case 'w':
			s = u_prnt(s, maxsize, timeptr->tm_wday, 1);
			maxsize -= s - olds;
			break;
		case 'W':
			s = u_prnt(s, maxsize,		/* ??? */
			    (timeptr->tm_yday+7-(timeptr->tm_wday+6)%7)/7,2);
			maxsize -= s - olds;
			break;
		case 'x':
			n = strftime(s, maxsize, "%a %b %d %Y", timeptr);
			maxsize -= n;
			s += n;
			break;
		case 'X':
			n = strftime(s, maxsize, "%H:%M:%S", timeptr);
			maxsize -= n;
			s += n;
			break;
		case 'y':
			s = u_prnt(s, maxsize, timeptr->tm_year % 100, 2);
			maxsize -= s - olds;
			break;
		case 'Y':
			s = u_prnt(s, maxsize, timeptr->tm_year + 1900, -1);
			maxsize -= s - olds;
			break;
		case 'Z':
			s = s_prnt(s, maxsize,
					__tzname[(timeptr->tm_isdst > 0)], -1);
			maxsize -= s - olds;
			break;
		case '%':
			*s++ = '%';
			maxsize--;
			break;
		default:
			/* A conversion error. Leave the loop. */
			while (*format) format++;
			break;
		}

	}
	if (maxsize) {
		*s = '\0';
		return s - firsts;
	}
	return 0;	/* The buffer is full */
}
