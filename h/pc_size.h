/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */

/*fundamental */
#define	sz_byte	1
#define	sz_bool	1
#define	sz_char	1

/* target machine characteristics */
/* variable (see pc.c) */
#define	sz_addr	sizes[0]
#define	sz_real	sizes[1]
#define	sz_head	sizes[2]
#define	sz_buff	sizes[3]
#define	sz_mset	sizes[4]
#define	sz_iset	sizes[5]
#define	sz_word	sizes[6]
#define	sz_int	sizes[7]
#define	sz_long	sizes[8]

#define	sz_last	8

#define	sz_proc	2*sz_addr
