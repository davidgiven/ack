/*
	Direct unchecked memory access
*/

/* $Id$ */

/*	The set of macros is neither systematic nor exhaustive; its contents
	were suggested by expediency rather than by completeness.
*/

/* Loading from memory */

#define	p_in_stack(a)	i2p((psize == 2) \
			? (btol(stack_loc(a)) | (btol(stack_loc(a+1))<<8)) \
			: (btol(stack_loc(a)) | (btol(stack_loc(a+1))<<8) | \
				(btol(stack_loc(a+2))<<16) | \
				(btol(stack_loc(a+3))<<24)))

#define	p_in_data(a)	i2p((psize == 2) \
			? (btol(data_loc(a)) | (btol(data_loc(a+1))<<8)) \
			: (btol(data_loc(a)) | (btol(data_loc(a+1))<<8) | \
				(btol(data_loc(a+2))<<16) | \
				(btol(data_loc(a+3))<<24)))

#define	p_in_text(a)	i2p((psize == 2) \
			? (btol(text_loc(a)) | (btol(text_loc(a+1))<<8)) \
			: (btol(text_loc(a)) | (btol(text_loc(a+1))<<8) | \
				(btol(text_loc(a+2))<<16) | \
				(btol(text_loc(a+3))<<24)))

#define	p_in_FRA(a)	i2p((psize == 2) \
			? (btol(FRA[a]) | (btol(FRA[a+1])<<8)) \
			: (btol(FRA[a]) | (btol(FRA[a+1])<<8) | \
				(btol(FRA[a+2])<<16) | \
				(btol(FRA[a+3])<<24)))

#define	w_in_stack(a)	((wsize == 2) \
			? (btol(stack_loc(a)) | (btos(stack_loc(a+1))<<8)) \
			: (btol(stack_loc(a)) | (btol(stack_loc(a+1))<<8) | \
				(btol(stack_loc(a+2))<<16) | \
				(btos(stack_loc(a+3))<<24)))

#define	w_in_data(a)	((wsize == 2) \
			? (btol(data_loc(a)) | (btos(data_loc(a+1))<<8)) \
			: (btol(data_loc(a)) | (btol(data_loc(a+1))<<8) | \
				(btol(data_loc(a+2))<<16) | \
				(btos(data_loc(a+3))<<24)))

#define	w_in_FRA(a)	((wsize == 2) \
			? (btol(FRA[a]) | (btos(FRA[a+1])<<8)) \
			: (btol(FRA[a]) | (btol(FRA[a+1])<<8) | \
				(btol(FRA[a+2])<<16) | \
				(btos(FRA[a+3])<<24)))

#define	LIN_in_stack(a)	((LINSIZE == 2) \
			? (btol(stack_loc(a)) | (btol(stack_loc(a+1))<<8)) \
			: (btol(stack_loc(a)) | (btol(stack_loc(a+1))<<8) | \
				(btol(stack_loc(a+2))<<16) | \
				(btol(stack_loc(a+3))<<24)))

