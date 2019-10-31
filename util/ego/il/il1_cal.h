/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/*  I N L I N E   S U B S T I T U T I O N
 *
 *  I L 1 _ C A L . C
 */

struct class {
	byte	src_class;
	byte	res_class;
};

typedef struct class *class_p;

extern struct class classtab[];

#define NOCLASS	0
#define CLASS1	1
#define CLASS2	2
#define CLASS3	3
#define CLASS4	4
#define CLASS5	5
#define CLASS6	6
#define CLASS7	7
#define CLASS8	8
#define CLASS9	9


void anal_cal(proc_p p, line_p call, bblock_p b, FILE *cf);
			/*
			 * analyze a call instruction;
			 * try to recognize the actual parameter
			 * expressions.
			 */
