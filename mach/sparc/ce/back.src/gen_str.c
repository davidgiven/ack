#include "header.h"

gen_str( s,n)
char *s;
int n;
{
	unsigned char c;
	switch ( cur_seg) {
	  case SEGTXT :
	  case SEGCON :
	  case SEGROM :
			fprint( codefile, "%s\"", STR_FMT);
			while (n--)
			{
				c= *s++;
				if (isprint(c) && c != '"' && c != '\\')
					fprint(codefile, "%c", c);
				else
					fprint(codefile, "\\%03o", c);
			}
			fprint( codefile, "\"\n");
			break;
	  case SEGBSS : bss( (arith) 1);
			break;
	  default : fprint( STDERR, "gen1 unkown seg %d\n", cur_seg);
	}
}
