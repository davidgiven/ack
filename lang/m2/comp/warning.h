/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 * Author: Ceriel J.H. Jacobs
 */

/* W A R N I N G   C L A S S E S */

/* $Id$ */

/* Warning classes, at the moment three of them:
   Strict (R)
   Ordinary (W)
   Old-fashioned(O)
*/

/* Bits for a bit mask: */

#define	W_ORDINARY	1
#define W_STRICT	2
#define W_OLDFASHIONED	4
#define W_ALWAYS	010	/* always given */

#define W_ALL		(W_ORDINARY|W_STRICT|W_OLDFASHIONED|W_ALWAYS)

#define W_INITIAL	(W_ORDINARY | W_OLDFASHIONED | W_ALWAYS)

/* The bit mask itself: */
extern int	warning_classes;
