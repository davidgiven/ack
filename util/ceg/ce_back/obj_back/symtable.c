#include <out.h>
#include <system.h>
#include "hash.h"
#include "header.h"
#include "back.h"

/* Findsym() manages the symbol table. It can be called in three ways. 
 * 1) findsym( "string", DEFINITION) : look if string is present in the
 * 	symol table. If not create a new entry: set index into the
 *	string_area. Return the index to the entry. Symbol_def() will
 *      the fields.
 * 2) findsym( "string", REFERENCE) : If string is present but never used
 *    (see 3)) make this name extern visible and set on_valu on -1.
 *    We use the on_valu field to distinguish between external visible 
 *    names that are defined here and those that are defined in another file.
 *    If the string is not present create a new entry and make the name extern
 *    visible and set on_valu on -1.
 *    If the name is present do nothing special.
 * 3) findsym( "string", STORE_STRING) : this call is made to save a
 *    copy action. The first time a name is encountered we store it
 *    immediately in the symbol table. If the name is not present we
 *    create a new entry and set the on_valu field on -2. In this
 *    way we can tell later that the name is not used yet. The index 
 *    is stored in the global varaible 'index_symbol_table' because it is
 *    very likely that the same string is in the next call to findsym.
 *    (After introducing a new name one does something with this name)
 */

int		string_lengte = 0,
		index_symbol_table = -1;

struct Hashitem  *Hashitems ; 

/*	MAXHASH must be a power of two ... */
#define MAXHASH	512
static int	 Hashtab[ MAXHASH];   
static int Hash();


int find_sym( sym, isdef)
char *sym;
int isdef;
{
	register struct outname *s;
	register struct Hashitem *ip;
	register int h;

	if (isdef != FORCE_DEF) {
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
		register char *p = sym, *q;

		s = symbol_table + ip->hs_nami;
		q = string_area + s->on_foff;
		while (*p == *q++) if (*p++ == '\0') {
		      	if ( (s->on_valu == -2) && (isdef == REFERENCE)) {
			          s->on_type = S_EXT; 
			          s->on_valu = -1;
		        }
		       	return ip->hs_nami;
		}
	    }
	}
	
	if ( nname >= size_symbol)  
		mem_symbol_hash();

	s = symbol_table + nname;

	if (isdef != FORCE_DEF) {
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
	}

	if ( sym == string) 
	        string += string_lengte;
	else {    /* zie C_fil, C_lin, C_lni */
		register char *p;

		string_lengte = 0;
		for( p=sym; *p != '\0' ; p++) {
			string_lengte++;
		}
		while ( (string + string_lengte - string_area) >= size_string) {
			mem_string();
		}
		for( p=sym; *p != '\0' ; p++) {
			*string++ = *p;
		}
	}
	if ( (string - string_area) >= size_string) {
		mem_string();
	}
	*string++ = '\0';
	s->on_foff = string - (string_lengte + 1) - string_area;

	return nname++;
}


static int Hash(sym)
	char *sym;
{
	register unsigned h;
	register char *s = sym;

	h = 0;
	while (*s) {
		h = (h << 2) + *s++;
	}
	return (h & (MAXHASH - 1));
}
