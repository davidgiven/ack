/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/*		keytab.c		*/
# include "Lpars.h"
# include <ctype.h>

# define NKEYWORDS	((sizeof keytab) / (sizeof *keytab))
# define MAXKEYLEN	8

typedef struct {
	int k_token;
	char *k_str;
} KTAB;

KTAB keytab[] = {
	{ AFTER,	"AFTER"	   }, { ALLOCATE,	"ALLOCATE" },
	{ ALT,		"ALT" 	   }, { AND,		"AND" 	   },
	{ ANY,		"ANY" 	   }, { BYTE,		"BYTE" 	   },
	{ CHAN,		"CHAN"	   }, { DEF,		"DEF"	   },
	{ FALSE,	"FALSE"	   }, { FOR,		"FOR"	   },
	{ IF,		"IF"	   }, { LOAD,		"LOAD"	   },
	{ NOT,		"NOT"	   }, { NOW,		"NOW"	   },
	{ OR,		"OR"	   }, { PAR,		"PAR"	   },
	{ PLACED,	"PLACED"   }, { PORT,		"PORT"	   },
	{ PRI,		"PRI"	   }, { PROC,		"PROC"	   },
	{ SEQ,		"SEQ"	   }, { SKIP,		"SKIP"	   },
	{ TABLE,	"TABLE"	   }, { TRUE,		"TRUE"	   },
	{ VALUE,	"VALUE"	   }, { VAR,		"VAR"	   },
	{ WAIT,		"WAIT"	   }, { WHILE,		"WHILE"	   },
};

/*
 *	The table of keywords is searched for the occurence of `str',
 *	if found the corresponding token number is returned,
 *	otherwise IDENTIFIER is the returned token number.
 */
keyword(str) char *str;
{
	register int high= NKEYWORDS-1;
	register int low= 0;
	register int i, cmp;
	char *lowerupper();
	register char *key;

	if ((key=lowerupper(str))==0) return IDENTIFIER;

	do {
		i= (high+low) / 2;
		if ((cmp= strcmp(key, keytab[i].k_str)) == 0) break;
		else if (cmp > 0) low= i+1;
		else high= i-1;
	} while (low <= high);
	
	return low<=high ? keytab[i].k_token : IDENTIFIER;
}

char *lowerupper(str) register char *str;
{
	static char keyword[MAXKEYLEN+1];
	register char *key=keyword;

	if (islower(*str)) {
		do {
			*key++ = toupper(*str);
			str++;
		} while (key<keyword+MAXKEYLEN && islower(*str));
	} else {
		do
			*key++ = *str++;
		while (key<keyword+MAXKEYLEN && isupper(*str));
	}
	*key=0;

	return *str==0 ? keyword : 0;
}

char *keyname(key) register int key;
{
	register KTAB *kp;

	for (kp= keytab; kp< keytab+NKEYWORDS; kp++)
		if (kp->k_token == key) return kp->k_str;

	return 0;
}
