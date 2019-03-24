
#ifndef __SYM_H_INCLUDED__
#define __SYM_H_INCLUDED__

extern unsigned short NLocals;	/* Number of local names to be saved. */
extern unsigned short NGlobals;	/* Number of global names. */

/** Enter a new name in the symbol table.*/
void entername(struct outname* name, int hashval);
/** Initialize the symbol table. All indices should be noticeably invalid. */
void init_symboltable(void);
/** Return the hash value of the string represented in p. */
int hash(register char* p);
/** Return the index of `name' in the symbol table. */
unsigned int indexof(struct outname *name);
/*
 * Search for `string' in the symboltable. The hash value of `string' is in
 * `hashval'. The linked list belonging to the entry of hashval
 * in the hash table is followed. If the names match, a pointer to the outname
 * in this element of the list is returned. When a match cannot be found,
 * NIL is returned.
 */
struct outname *searchname(char *string, int hashval);



#endif /* __SYM_H_INCLUDED__ */
