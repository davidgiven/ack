#include <out.h>
#include <system.h>
#include "hash.h"
#include "header.h"
#include "back.h"


int		string_lengte = 0,
		index_symbol_table = -1;

struct Hashitem  *Hashitems ; 
int		 Hashtab[ MAXHASH];   


int find_sym( sym, isdef)
char *sym;
int isdef;
{
	register char *p;
	register struct outname *s;
	register struct Hashitem *ip;
	register int h;

	if ( index_symbol_table != -1 )	{
		s = symbol_table + index_symbol_table;
		if ( sym == s->on_foff + string_area)  {
			if ( (s->on_valu == -2) && ( isdef == REFERENCE)) {
				s->on_type = S_EXT; 
				s->on_valu = -1;
			}
	        	return( index_symbol_table);
		}
	}

	h = Hash(sym);
	for ( ip = Hashtab[h] + Hashitems ; ip != Hashitems; 
					  ip = (ip->hs_next) + Hashitems) {
		s = symbol_table + ip->hs_nami;
		if (strcmp(sym, (s->on_foff) + string_area) == 0) {
		      	if ( (s->on_valu == -2) && (isdef == REFERENCE)) {
			          s->on_type = S_EXT; 
			          s->on_valu = -1;
		        }
		       	return ip->hs_nami;
		}
	}
	
	if ( nname >= size_symbol)  
		mem_symbol_hash();

	s = symbol_table + nname;
	ip = Hashitems + nname + 1;  /* skip the first entry */

	if (isdef == REFERENCE)  {
		s->on_type = S_EXT; 
		s->on_valu = -1;
	}
	if (isdef == STORE_STRING) {
		s->on_type = S_UND;
		s->on_valu = -2; 
	}

	ip->hs_nami = nname;
	ip->hs_next = Hashtab[h];
	Hashtab[h] = ip - Hashitems;

	if ( sym == string) 
	        string += string_lengte;
	else {    /* zie C_fil, C_lin, C_lni */
		string_lengte = 0;
		for( p=sym; *p != '\0' ; p++) {
			if ( (string - string_area) >= size_string) 
				mem_string();
			*string++ = *p;
			string_lengte++;
		}
	}
	if ( (string - string_area) >= size_string) 
		mem_string();
	*string++ = '\0';
	s->on_foff = string - (string_lengte + 1) - string_area;

	return nname++;
}


int Hash(sym)
char *sym;
{
	register unsigned h;
	register c;

	h = 0;
	while (c = *sym++) {
		h <<= 2;
		h += c;
	}
	return (h % MAXHASH);
}
