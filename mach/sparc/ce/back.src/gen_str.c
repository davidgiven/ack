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
			fprintf( codefile, "%s\"", STR_FMT);
			while (n--)
			{
				c= *s++;
				if (isprint(c) && c != '"' && c != '\\')
					fprintf(codefile, "%c", c);
				else
					fprintf(codefile, "\\%03o", c);
			}
			fprintf( codefile, "\"\n");
			break;
	  case SEGBSS : bss( (arith) 1);
			break;
	  default : fprintf( stderr, "gen1 unkown seg %d\n", cur_seg);
	}
}
