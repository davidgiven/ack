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
	if (result)
		return result;
	result = getenv("TMP");
	if (result)
	    return result;
	/* DOS compatible systems */
	result = getenv("temp");
	if (result)
		return result;
	result = getenv("TEMP");
	if (result)
		return result;
	/* Then try and guess. */
	#if defined WIN32
		return ".";
	#else
		return "/tmp";
	#endif
}


