/* fundamental */
#define	sz_byte	1
#define	sz_bool	1
#define	sz_char	1

/* fixed for the time being */
#define	sz_word	2
#define	sz_int	2
#define	sz_long	4

/* variable (see pc.c) */
#define	sz_addr	sizes[0]
#define	sz_real	sizes[1]
#define	sz_head	sizes[2]
#define	sz_buff	sizes[3]
#define	sz_mset	sizes[4]
#define	sz_iset	sizes[5]

#define	sz_last	5

#define	sz_proc	2*sz_addr
