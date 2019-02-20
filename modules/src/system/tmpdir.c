/*  Copyright (c) 2019. See the file "Copyright" in
 *  the root directory for more information.
 *
 *  Created on: 2019-02-09
 *  
 */
#include <stdlib.h>

char* sys_gettmpdir(void)
{
	char* result = 0;
	/* Check the UNIX temporary directory */
	result = getenv("TMPDIR");
	if (result != 0)
		return result;
	result = getenv("TMP");
	if (result != 0)
	    return result;
	/* DOS compatible systems */
	result = getenv("TEMP");
	if (result != 0)
		return result;
	/* Then try current directory */
	return ".";
}


