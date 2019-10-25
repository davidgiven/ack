/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#include "top.h"
#include "queue.h"

void empty_queue(queue q)
{
	q->head = q->tail = (instr_p) 0;
	q->qlen = 0;
}

int empty(queue q)
{
	return q->qlen == 0;
}

void remove_head(queue q)
{
	if ( (q->head = q->head->fw) == (instr_p) 0) {
		q->tail = (instr_p) 0;
	} else {
		q->head->bw = (instr_p) 0;
	}
	q->qlen--;
}

void add(queue q, instr_p instr)
{
	if (q->qlen++ == 0) {
		q->head = q->tail = instr;
		instr->bw = (instr_p) 0;
	} else {
		q->tail->fw = instr;
		instr->bw = q->tail;
		q->tail = instr;
	}
	instr->fw = (instr_p) 0;
}

void insert(queue q, instr_p instr)
{
	if (q->qlen++ == 0) {
		q->head = q->tail = instr;
		instr->fw = (instr_p) 0;
	} else {
		q->head->bw = instr;
		instr->fw = q->head;
		q->head = instr;
	}
	instr->bw = (instr_p) 0;
}

void join_queues(queue q1, queue q2)
{
	if (q1->qlen > 0) {
		q2->qlen += q1->qlen;
		q1->tail->fw = q2->head;
		if (q2->qlen > 0) {
			q2->head->bw = q1->tail;
		} else {
			q2->tail = q1->tail;
		}
		q2->head = q1->head;
		empty_queue(q1);
	}
}
