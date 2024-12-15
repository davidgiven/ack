#ifndef TOKENNAME_H
#define TOKENNAME_H

struct tokenname
{ /*	Used for defining the name of a
token as identified by its symbol
*/
	int tn_symbol;
	char* tn_name;
};

extern char* symbol2str(int tok);
extern void reserve(struct tokenname* resv);

#endif