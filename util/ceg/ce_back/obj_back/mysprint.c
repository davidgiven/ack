#include <system.h>
#include "data.h"
#include <varargs.h>

/* Mysprint() stores the string directly in the string_arae. This saves
 * a copy action. It is assumed that the strings stored in the string-table
 * are never longer than MAXSTRLEN bytes.
 */

#define MAXSTRLEN	1024

/*VARARGS*/
int mysprint(va_alist)
	va_dcl
{
	char *fmt;
	va_list args;
	int retval;

	va_start(args);
	fmt = va_arg(args, char *);
	while (string + MAXSTRLEN - string_area > size_string)
		mem_string();
	retval = _format(string, fmt, args);
	string[retval] = '\0';
	va_end(args);
	return retval;
}
