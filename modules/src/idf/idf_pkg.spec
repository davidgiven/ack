/* $Header$ */
/* IDENTIFIER DESCRIPTOR */

/* This a generic package for maintaining a name list */

/* Instantiation parameters, supplied by #define, are :
	IDF_TYPE: the type of the user-defined part of the idf-structure,
	IDF_NAME: the selector name for this field in the idf_structure, and
	IDF_HSIZE: the number of significant characters for hashing
*/

#ifndef IDF_NAME
#define IDF_NAME id_user
#endif

struct idf	{
	struct idf *id_next;	/* links idf-structures together */
	char *id_text;		/* string representing the name */
#ifdef IDF_TYPE
	IDF_TYPE IDF_NAME;	/* user defined type and selector */
#endif
};

/*	init_idf()

	Initializes the namelist. Empty for now.
*/
#define			init_idf()

/*	struct idf * str2idf(tg, cp)
		char *tg;
		int cp;

	Adds the string indicated by "tg" to the namelist, and returns a
	pointer to the entry.
	If cp > 0, a copy of tg is made for id_text, otherwise tg itself
	is used.
	If cp < 0, the string is not entered, but only looked for.
*/
extern struct idf *	str2idf();

#define	findidf(tg)	str2idf(tg, -1)
