/* $Header$ */

/* Structure for information about files. This information consists of three
   parts:
   - file name and directory
   - mapping of line numbers to offsets in file
   - mapping of object adresses to lines in file and vice versa
*/

#define LOGHSIZ		6		/* make sure HSIZ is a power of 2 */
#define HSIZ		(1 << LOGHSIZ)
#define	HASH(line)	((line) & (HSIZ-1))

typedef struct file {
	struct symbol	*f_sym;
	struct symbol	*f_base;
	char		*f_fullname;	/* name including directory */
	struct scope	*f_scope;	/* reference to scope of this file */
	t_lineno	f_nlines;	/* number of lines in file */
	union {
	  long		*ff_linepos;	/* positions of lines in file */
	  struct file	*ff_next;	/* only for BINCL, EINCL */
	} f_x;
#define f_linepos	f_x.ff_linepos
#define f_next		f_x.ff_next
	struct outname	*f_line_addr[HSIZ];
					/* hash table, mapping line numbers to
					   outname structures. Collisions are
					   resolved by chaining:
					*/
#define next_outname(n)		((struct outname *) ((n)->on_mptr))
#define setnext_outname(n,m)	((n)->on_mptr = (char *) (m))
} t_file, *p_file;

/* ALLOCDEF "file" 10 */
