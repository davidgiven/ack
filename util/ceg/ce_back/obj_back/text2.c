#include "mach.h"
#include "back.h"

text2( w)
TWO_BYTES w;
{
	if ((_text_cnt -= 2) < 0) mem_text();
#ifdef BYTES_REVERSED
	*text++ = ( unsigned short)w>>8;
	*text++ = w;
#else
	*text++ = w;
	*text++ = ( unsigned short)w>>8;
#endif
}
