/* $Header$ */
/* IDENTIFIER STACK DEFINITIONS */

/*	The identifier stack is implemented as a stack of sets.
	The stack is implemented by a doubly linked list,
	the sets by singly linked lists.
*/

struct stack_level	{
	struct stack_level *next;
	struct stack_level *sl_next;		/* upward link		*/
	struct stack_level *sl_previous;	/* downward link	*/
	struct stack_entry *sl_entry;		/* sideward link	*/
	arith sl_local_offset;		/* @ for first coming object	*/
	arith sl_max_block;		/* maximum size of sub-block	*/
	int sl_level;
};


/* allocation definitions of struct stack_level */
/* ALLOCDEF "stack_level" */
extern char *st_alloc();
extern struct stack_level *h_stack_level;
#define	new_stack_level() ((struct stack_level *) \
		st_alloc((char **)&h_stack_level, sizeof(struct stack_level)))
#define	free_stack_level(p) st_free(p, h_stack_level, sizeof(struct stack_level))


struct stack_entry	{
	struct stack_entry *next;
	struct idf *se_idf;
};


/* allocation definitions of struct stack_entry */
/* ALLOCDEF "stack_entry" */
extern char *st_alloc();
extern struct stack_entry *h_stack_entry;
#define	new_stack_entry() ((struct stack_entry *) \
		st_alloc((char **)&h_stack_entry, sizeof(struct stack_entry)))
#define	free_stack_entry(p) st_free(p, h_stack_entry, sizeof(struct stack_entry))


extern struct stack_level *local_level;
extern struct stack_level *stack_level_of();
extern int level;
