/* $Header$ */
/* FAULTY DEFINITIONS */

#define	faulty(tp)	((tp)_faulty(__FILE__, __LINE__))
#define	fault()		(_faulty(__FILE__, __LINE__))
