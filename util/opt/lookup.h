/* $Header$ */

#define IDL	8

struct sym {
	sym_p	s_next;
	char	s_name[IDL];
	offset	*s_rom;
	short	s_flags;
	short	s_frag;
	offset	s_value;
};

/* contents of .s_flags */
#define SYMPRO		000001
#define SYMGLOBAL	000002
#define SYMKNOWN	000004
#define SYMOUT		000010
#define SYMDEF		000020

#define NSYMHASH	127
extern sym_p symhash[NSYMHASH],symlookup();
#define OCCURRING	0
#define DEFINING	1
#define NOTHING		2
