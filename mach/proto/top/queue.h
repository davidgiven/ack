/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
typedef struct queue_t *queue;

struct queue_t {
	instr_p	head;
	instr_p	tail;
	int	qlen;
};

#define qhead(q)	(q)->head
#define qlength(q)	(q)->qlen
#define next(x)		(x)->fw
