/* M I S C E L L A N E O U S */

/* $Header$ */

/* ALLOCDEF "id_list" */

#define is_anon_idf(x)	((x)->id_text[0] == '#')

extern struct idf
	*gen_anon_idf();
