typedef struct glosym {
	struct glosym *gl_next;
	string	       gl_name;
	word	       gl_rom[MAXROM+1];
} glosym_t,*glosym_p;

glosym_p lookglo();
