/* $Header$ */

/* scope structure */

typedef struct scope {
  struct scope	*sc_static_encl;	/* linked list of enclosing scopes */
  struct symbol *sc_symbs;		/* symbols defined in this scope */
  struct symbol *sc_definedby;		/* symbol defining this scope */
  long		sc_start;		/* start address of code of this scope */
  long		sc_bp_opp;		/* first breakpoint opportunity */
  int		sc_bp_lineno;		/* linenumber belonging to this bp. */
  short		sc_proclevel;		/* proc level of this scope */
  char		sc_has_activation_record;
} t_scope, *p_scope;

extern p_scope PervasiveScope, CurrentScope, FileScope;

/* extern	init_scope();
   Initializes the scope routines.
*/
extern	init_scope();

/* extern	open_scope(struct symbol *name, int has_activation);
   Opens a new scope and assigns it to CurrentScope; The new scope is defined
   by 'name' and if 'has_activation' is set, it has an activation record.
*/
extern	open_scope();

/* extern	close_scope();
   Closes the current scope; CurrentScope becomes the statically enclosing
   scope.
*/
extern	close_scope();

/* extern	add_scope_addr(p_scope sc);
   Adds scope 'sc' to the list of scopes that have an address at runtime.
*/
extern	add_scope_addr();

/* extern p_scope	get_scope_from_addr(t_addr a);
   Returns the scope of the code at address 'a', or 0 if it could not be found.
*/
extern p_scope	get_scope_from_addr();

/* extern p_scope	get_next_scope_from_addr(t_addr a);
   Returns the scope following the one of the code at address 'a',
   and that has an activation record,
   or 0 if it could not be found.
*/
extern p_scope	get_next_scope_from_addr();

/* extern p_scope	base_scope(p_scope sc);
   Returns the closest enclosing scope of 'sc' that has an activation record.
*/
extern p_scope	base_scope();

/* extern int	scope_encloses(p_scope scope, from_scope);
   Returns 1 if scope encloses from from_scope, 0 otherwise.
*/
extern int	scope_encloses();
