/* $Header$ */
/*		L A B E L   D E F I N I T I O N				*/

#define	label		unsigned int
#define	NO_LABEL	(label) 0

extern label lab_count;
#define	text_label()	(lab_count++)		/* returns a new text label */

extern label datlab_count;
#define	data_label()	(datlab_count++)	/* returns a new data label */
