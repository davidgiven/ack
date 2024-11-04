/* M I S C E L L A N E O U S */

struct node;

#define is_anon_idf(x)		((x)->id_text[0] == '#')
#define id_not_declared(x)	(not_declared("identifier", (x), ""))

extern char *gen_proc_name(struct idf *id, int inp);
extern struct idf *gen_anon_idf(void);
extern void not_declared(char *what, struct node *id, char *where);

