
/* variable types (stored in the   vtype  field of   expptr)
 * numeric assumptions:
 *	int < reals < complexes
 *	TYDREAL-TYREAL = TYDCOMPLEX-TYCOMPLEX
 */

#define TYUNKNOWN 0
#define TYADDR 1
#define TYSHORT 2
#define TYLONG 3
#define TYREAL 4
#define TYDREAL 5
#define TYCOMPLEX 6
#define TYDCOMPLEX 7
#define TYLOGICAL 8
#define TYCHAR 9
#define TYSUBR 10
#define TYERROR 11
#define TYCILIST 12
#define TYICILIST 13
#define TYOLIST 14
#define TYCLLIST 15
#define TYALIST 16
#define TYINLIST 17
#define TYVOID 18
#define TYLABEL 19
#define TYFTNLEN 20
/* TYVOID is not in any tables. */

/* NTYPES, NTYPES0 -- Total number of types, used to allocate tables indexed by
   type.  Such tables can include the size (in bytes) of objects of a given
   type, or labels for returning objects of different types from procedures
   (see array   rtvlabels)   */

#define NTYPES TYVOID
#define NTYPES0 TYCILIST
#define TYBLANK TYSUBR		/* Huh? */

