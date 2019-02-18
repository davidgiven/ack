/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Id$ */
/*		L A B E L   D E F I N I T I O N				*/
#ifndef LABEL_H_
#define LABEL_H_

#include <em_label.h>		/* obtain definition of "label" */

#define	NO_LABEL	(label) 0

extern label lab_count;
#define	text_label()	(lab_count++)		/* returns a new text label */

extern label datlab_count;
#define	data_label()	(datlab_count++)	/* returns a new data label */

#define define_label(idf) enter_label(idf, 1);
	/*	The identifier idf is defined as a label. If it is new,
		it is entered into the idf list with the largest possible
		scope, i.e., on the lowest possible level.
	*/

#define apply_label(idf) enter_label(idf, 0);
	/*	The identifier idf is applied as a label. It may or may
		not be there, and if it is there, it may be from a
		declaration or another application.
	*/

struct idf;

void enter_label(register struct idf *idf, int defining);
void unstack_label(register struct idf *idf);

#endif /* LABEL_H_ */
