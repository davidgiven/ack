/* S O M E   G L O B A L   V A R I A B L E S */

extern char options[];		/* indicating which options were given */
extern char *input;		/* name of required filevariable */
extern char *output;		/* name of required filevariable */

extern struct def *program;	/* definition of the program compiled */

extern int proclevel;		/* nesting level of procedures */
extern int err_occurred;

extern label data_label;
extern label text_label;
