/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 * Author: Ceriel J.H. Jacobs
 */

/*  F I L E   L I S T   S T R U C T U R E   */

struct file_list {
	char		*a_filename;	/* name of file */
	char		*a_dir;		/* directory in which it resides */
	struct idf	*a_idf;		/* its idf-structure */
	struct file_list *a_next;	/* next in list */
	char		a_notfound;	/* could not open ... */
};

#define f_walk(list, ctrl) \
	for (ctrl = (list); ctrl; ctrl = ctrl->a_next)

#define f_filename(a)	((a)->a_filename)
#define f_idf(a)	((a)->a_idf)
#define f_dir(a)	((a)->a_dir)
#define f_notfound(a)	((a)->a_notfound)
