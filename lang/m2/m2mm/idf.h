/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 * Author: Ceriel J.H. Jacobs
 */

/* U S E R   D E C L A R E D   P A R T   O F   I D F */

/* $Id$ */

#include "file_list.h"

struct lnk {
	struct lnk *lnk_next;
	struct idf *lnk_imp;
};

struct id_u {
	int id_res;
	int id_tp;		/* PROGRAM OR IMPLEMENTATION OR DEFINITION */
	struct lnk *id_defimp;	/* imported by definition module */
	struct lnk *id_modimp;	/* imported by implementation module */
	char *id_d;		/* directory */
	struct file_list *id_mdep;	/* module depends on: */
	struct file_list *id_ddep;	/* definition module depends on: */
	char *id_df;		/* name of definition module */
};

#define IDF_TYPE	struct id_u
#define id_reserved	id_user.id_res
#define id_type		id_user.id_tp
#define id_defimports	id_user.id_defimp
#define id_modimports	id_user.id_modimp
#define id_dir		id_user.id_d
#define id_mdependson	id_user.id_mdep
#define id_ddependson	id_user.id_ddep
#define id_def		id_user.id_df

#include	<idf_pkg.spec>
