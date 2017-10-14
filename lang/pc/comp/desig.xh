/* D E S I G N A T O R   D E S C R I P T I O N S */

/* Generating code for designators is not particularly easy, especially if
   you don't know whether you want the address or the value.
   The next structure is used to generate code for designators.
   It contains information on how to find the designator, after generation
   of the code that is common to both address and value computations.
*/

struct desig	{
	int	dsg_kind;
#define DSG_INIT	0	/* don't know anything yet */
#define DSG_LOADED	1	/* designator loaded on top of the stack */
#define DSG_PLOADED	2	/* designator accessible through pointer on
				   stack, possibly with an offset
				*/
#define DSG_FIXED	3	/* designator directly accessible */
#define DSG_PFIXED	4	/* designator accessible through directly
				   accessible pointer
				*/
#define DSG_INDEXED	5	/* designator accessible through array
				   operation. Address of array descriptor on
				   top of the stack, index beneath that, and
				   base address beneath that
				*/
	arith	dsg_offset;	/* contains an offset for PLOADED,
				   or for FIXED or PFIXED it contains an
				   offset from dsg_name, if it exists,
				   or from the current Local Base
				*/
	char	*dsg_name;	/* name of global variable, used for
				   FIXED and PFIXED
				*/
	struct def *dsg_def;	/* def structure associated with this
				   designator, or 0
				*/
	int dsg_packed;		/* designator is packed or not */
};

/* The next structure describes the designator in a with-statement.
   We have a linked list of them, as with-statements may be nested.
*/

struct withdesig	{
	struct withdesig *w_next;
	struct scope *w_scope;	/* scope in which fields of this record
				   reside
				*/
	struct desig w_desig;	/* a desig structure for this particular
				   designator
				*/
};

/* ALLOCDEF "withdesig" 5 */

extern struct withdesig	*WithDesigs;
extern struct desig	InitDesig;

#define NO_LABEL	((label) 0)
