/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#include "debug.h"
struct macro *h_macro = 0;
struct mlist *h_mlist = 0;
#ifdef DEBUG
int cnt_macro = 0;
int cnt_mlist = 0;
#endif
