/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* h a s h . c
 *
 * maintains the the lists of hashed patterns
 * Functions : addtohashtable() and printhashtable()
 */
# include <stdio.h>
# include "misc.h"

struct hlist {			/* linear list of pattern numbers */
    int	h_patno;
    struct hlist *h_next;
};

static struct hlist *hashtable[129];	/* an array of ptr's to these lists,
					 * the index in the array is the
					 * result of hashing
					 */

static unsigned
hash(string) char *string; {
	register char *p;
	register unsigned i,sum;

	if (strcmp(string,"ANY") == 0) return 128;
	for (sum=i=0,p=string;*p;i += 3)
		sum += (*p++)<<(i&03);
	return sum % 128;
}


addtohashtable(s,n) char *s; {
    /*
     * Add a new pattern number to the hashtable. 
     * s is the key, n the pattern number
     */
    unsigned hval;
    register struct hlist *p;
    char *malloc();

    hval = hash(s);
    p = (struct hlist *) malloc(sizeof *p);
    p->h_patno = n;
    /*
     * Now insert in front of the list 
     * This way, the list will be sorted from high to low, which is the
     * wrong way around, but easy
     */
    p->h_next = hashtable[hval];
    hashtable[hval] = p;
}

static
prhlist(p) struct hlist *p; {
    /*
     * Print a list in reversed order (see comment above)
     */

    if (p) {
	prhlist(p->h_next);
	fprintf(genc,"%d, ",p->h_patno - 1);
    }
}
 
printhashtable() {
    /*
     * Print the linear lists, and also output an array of
     * pointers to them
     */
    register i;
    register struct hlist *p;

    for (i = 1; i <= 128; i++) {
	fprintf(genc,"int hash%d[] = { ",i);
	prhlist(hashtable[i-1]);
	fputs("-1};\n",genc);
    }
    fputs("int hashany[] = { ", genc);
    prhlist(hashtable[128]);
    fputs("-1 };\n",genc);
    fputs("int *hashtab[] = {\n",genc);
    for (i = 1; i <= 128; i++) fprintf(genc,"\thash%d,\n",i);
    fputs("\thashany\n};\n",genc);
}
