#ifndef LANGDEP_H
#define LANGDEP_H

/* language-dependent routines and formats, together in one structure: */

struct tree;
struct langdep
{
	/* language info: */
	int has_bool_type; /* set if language has a boolean type */

	/* formats (for fprintf): */
	char* decint_fmt; /* decimal ints (format for long) */
	char* octint_fmt; /* octal ints (format for long) */
	char* hexint_fmt; /* hexadecimal ints (format for long) */
	char* uns_fmt; /* unsigneds (format for long) */
	char* addr_fmt; /* address (format for long) */
	char* real_fmt; /* real (format for double) */

	/* display openers and closers: */
	char* open_array_display;
	char* close_array_display;
	char* open_struct_display;
	char* close_struct_display;
	char* open_set_display;
	char* close_set_display;

	/* language dependant routines: */
	void (*printstring)(FILE* f, char* s, int len);
	void (*printchar)(int c);
	long (*arrayelsize)(long size);
	int (*binop_prio)(int op);
	int (*unop_prio)(int op);
	int (*get_string)(int c);
	int (*get_name)(int c);
	int (*get_number)(int c);
	int (*get_token)(int c);
	void (*printop)(FILE* f, struct tree* p);
	void (*fix_bin_to_pref)(struct tree* p);
};

extern struct langdep *m2_dep, *c_dep, *pascal_dep, *currlang;

extern void find_language(char* suff);
extern void init_languages(void);
extern int LLlex(void);
extern void Commands(void);

extern int get_string(long size, t_addr from, char* to);
extern int get_bytes(long size, t_addr from, char* to);

#endif
