#include <stdio.h>
#include "em.h"

arith cur_pos = 0;

dist( lab)
char *lab;
{
	out( "$%s$ - %ld", lab, cur_pos);
}

#define MAX_LABEL	10
#define	TRUE		1
#define FALSE		0


struct t_label  {
			char *lab;
			arith position;
		 };

struct t_label  label_list[ MAX_LABEL];
int n_labs = 0;

handle_label( lab)
char *lab;
{
	char *Salloc();

	if ( n_labs >= MAX_LABEL)
		error( "Too many labels!!\n");
	else {
		label_list[ n_labs].lab = Salloc( lab, strlen( lab) + 1);
		label_list[ n_labs++].position = cur_pos;
	}
	process_label( lab);
}


relocate( f)
FILE *f;
{
	char buf[256];
	int i;

	fscanf( f, " %[^$]$", buf);
	for ( i = 0; i < n_labs; i++)
		if ( strcmp( buf, label_list[i].lab) == 0) {
			out( "%ld", label_list[i].position);
			return;
		}
	error( "relocate() : can't find local label %s\n", buf);
}

#include <system.h>

File *oldout;
extern File *outfile;

back_patch()
{
	FILE *save;
	char c, c1;

	sys_close( outfile);
	outfile = oldout;
	save = fopen( ".tmp", "r");

	while ( ( c = getc( save)) != EOF)
		if ( c == '$')
			relocate( save);
		else if ( c == '/') {		/* skip comments */
			c = getc( save);
			out( "/%c", c);
			if ( c == '*') {
				c = getc( save);
				do  {
					out( "%c", c);
					c1 = c;
					c = getc( save);
				}
				while ( !( c1 == '*' && c == '/'));
				out( "%c", c);
			}
		}
		else
			out( "%c", c);
	fclose( save);
}


save_output()
{
	extern arith cur_pos;
	extern int n_labs;

	oldout = outfile;
	if ( ! sys_open( ".tmp", OP_WRITE, &outfile))
		fprint( STDERR, "can't open .tmp\n");
	cur_pos = 0;
	n_labs = 0;	/* geheugen-ruimte teruggeven? */
}
