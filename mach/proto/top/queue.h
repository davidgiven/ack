typedef struct item *item_p;
typedef struct queue_t *queue;

struct queue_t {
	instr_p	head;
	instr_p	tail;
	int	qlen;
};

#define qhead(q)	(q)->head
#define qlength(q)	(q)->qlen
#define next(x)		(x)->fw
