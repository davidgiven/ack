#include "mach.h"
#include "back.h"

rom2( w)
TWO_BYTES w;
{
	if ((data_cnt -= 2) < 0) mem_data();
#ifdef BYTES_REVERSED
	*data++ = w>>8;
	*data++ = w;
#else
	*data++ = w;
	*data++ = w>>8;
#endif
}
