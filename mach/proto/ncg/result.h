/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Header$ */

struct result {
	int	e_typ;		/* EV_INT,EV_REG,EV_STR */
	union {
		word e_con;
		int e_reg;
		addr_t e_addr;
	} e_v;			/* value */
};

#define EV_UNDEF 0
#define EV_INT	1
#define EV_REG	2
#define EV_ADDR	3

typedef struct result result_t;
