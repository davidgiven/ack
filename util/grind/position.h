/* $Id$ */

/* maps from address to filename-lineno pair and reverse,
   maps from filename-lineno pair or address to scope.
*/

typedef unsigned int	t_lineno;
typedef long		t_addr;
#define ILL_ADDR	((t_addr) -1)
#define NO_ADDR		((t_addr) 0)

typedef struct pos {
  t_lineno	lineno;
  char		*filename;
} t_position, *p_position;

/* extern p_position	get_position_from_addr(t_addr t);
   Returns a pointer to a structure containing the source position of the code
   at address 't'.  0 is returned if no source position could be found.
*/
extern p_position	get_position_from_addr();

/* extern t_addr	get_addr_from_position(p_position p);
   Returns the address of the code at position 'p', or ILL_ADDR if it could
   not be found. If there is no symbolic information for the filename in
   position 'p', an error message will be given.
*/
extern t_addr		get_addr_from_position();

/* extern	add_position_addr(char *filename, struct outname *n);
   Adds the ('filename','n'->on_desc),'n'->on_valu pair to the mapping information.
*/
extern 			add_position_addr();

/* extern p_position print_position(t_addr a, int print_function);
   Prints position 'a' and returns it. If 'print_function' is set,
   an attempt is made to print the function name as well.
*/
extern p_position	print_position();
