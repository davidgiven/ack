/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 * Author: Ceriel J.H. Jacobs
 */

/* M I S C E L L A N E O U S */

/* $Id$ */

#define is_anon_idf(x)		((x)->id_text[0] == '#')
#define id_not_declared(x)	(not_declared("identifier", (x), ""))

/* Forward declarations. */
typedef struct idf t_idf;
typedef struct node t_node;

void match_id(register t_idf *id1, t_idf *id2);
t_idf *gen_anon_idf(void);
void not_declared(char *what, t_node *id, char *where);
