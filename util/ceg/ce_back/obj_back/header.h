/* Constants and a macro that are global to the back library. */

#define SYMBOL_DEFINITION	1
#define REFERENCE		2
#define STORE_STRING		3
#define FORCE_DEF		4

#define conv_seg( s)	( ( s == SEGROM) ? SEGCON : s)
