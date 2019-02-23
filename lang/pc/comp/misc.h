/* M I S C E L L A N E O U S */

struct node;

#define is_anon_idf(x)		((x)->id_text[0] == '#')
#define id_not_declared(x)	(not_declared("identifier", (x), ""))

extern struct idf
	*gen_anon_idf();

extern char 
	*gen_proc_name();

void not_declared(char *what, register struct node *id, char *where);

extern char *symbol2str();

