/* $Header$ */

/*
 * Offsets of the pieces of the input module in core.
 */

#define IND_HEAD	((ind_t)0)
#define IND_SECT(x)	(IND_HEAD    +		      sizeof(struct outhead))
#define IND_NAME(x)	(IND_SECT(x) + (x).oh_nsect * sizeof(struct outsect))
#define IND_CHAR(x)	(IND_NAME(x) + (x).oh_nname * sizeof(struct outname))
#ifdef SYMDBUG
#define OFF_DBUG(x)	(OFF_CHAR(x) + (x).oh_nchar)
#endif SYMDBUG
