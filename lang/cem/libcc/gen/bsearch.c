/*  bsearch(3)
 *
 *  Author: Terrence Holm          Aug. 1988
 *
 *
 *  Performs a binary search for a given <key> within a sorted
 *  table. The table contains <count> entries of size <width>
 *  and starts at <base>.
 *
 *  Entries are compared using keycmp( key, entry ), each argument
 *  is a (char *), the function returns an int < 0, = 0 or > 0
 *  according to the order of the two arguments.
 *
 *  Bsearch(3) returns a pointer to the matching entry, if found,
 *  otherwise NULL is returned.
 */

#define  NULL	(char *) 0


char *bsearch( key, base, count, width, keycmp )
  char *key;
  char *base;
  unsigned int count;
  unsigned int width;
  int  (*keycmp)();

  {
  char *mid_point;
  int  cmp;

  while ( count > 0 )
    {
    mid_point = base + width * (count >> 1);

    cmp = (*keycmp)( key, mid_point );

    if ( cmp == 0 )
	return( mid_point );

    if ( cmp < 0 )
	count >>= 1;
    else
	{
	base  = mid_point + width;
	count = (count - 1) >> 1;
	}
    }

  return( NULL );
  }
