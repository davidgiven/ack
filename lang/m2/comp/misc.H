/* M I S C E L L A N E O U S */

/* $Header$ */

/*	Structure to link idf structures together
*/
struct id_list {
	struct id_list *next;
	struct idf *id_ptr;
};

/* ALLOCDEF "id_list" */
