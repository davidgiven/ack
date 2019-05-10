/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 * Author: Ceriel J.H. Jacobs
 */

/* U S E R   D E C L A R E D   P A R T   O F   I D F */

/* $Id$ */

struct id_u {
	int id_res;
	struct def *id_df;
};

#define IDF_TYPE	struct id_u
#define id_reserved	id_user.id_res
#define id_def		id_user.id_df

#include	<idf_pkg.spec>


