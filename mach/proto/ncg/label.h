/*
 * This file contains the structures that hold information about the labels.
 * The height and the fallthrough functions of the table. 
 *
 * Author: Hans van Eck. 
 */

typedef struct label *label_p;

typedef struct label {
	label_p	lb_next;
	word	lb_number;
	int	lb_height;
	short	lb_fallthrough;
    } label_t;

extern label_p get_label();
