#define RCSID0 "$Header$"

/*
** Zilog z8000 machine dependent options
*/
#define THREE_PASS
#define BYTES_REVERSED
#define WORDS_REVERSED
#define LISTING
#define ASLD
#undef ALIGNSECT
#define ALIGNSECT	2
#undef valu_t
#define valu_t long
#undef addr_t
#define addr_t long
