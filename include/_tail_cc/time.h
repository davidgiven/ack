/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */

#ifndef _TIME_H
#define _TIME_H

struct tm {
  int tm_sec;			/* seconds after the minute [0, 59] */
  int tm_min;			/* minutes after the hour [0, 59] */
  int tm_hour;			/* hours since midnight [0, 23] */
  int tm_mday;			/* day of the month [1, 31] */
  int tm_mon;			/* months since January [0, 11] */
  int tm_year;			/* years since 1900 */
  int tm_wday;			/* days since Sunday [0, 6] */
  int tm_yday;			/* days since January 1 [0, 365] */
  int tm_isdst;			/* Daylight Saving Time flag */
};

extern struct tm *gmtime();
extern struct tm *localtime();
extern char *asctime();
extern char *ctime();

#endif /* _TIME_H */
