/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Header$ */
struct id_usr {
	struct macro *idu_macro;
	int idu_resmac;
};

#define IDF_TYPE struct id_usr
#define IDF_HSIZE 6
#define id_macro id_user.idu_macro
#define id_resmac id_user.idu_resmac

#include <idf_pkg.spec>
