/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
struct replicator {	/* Info transferred from rep_init to rep_test */
	int counter;
	int BEGIN;
	int END;
};

void rep_init(), rep_test();

void code_val(), code_addr(), code_void();
void code_assignment(), code_input(), code_any(), code_output();

void code_bool();
#define positive 1	/* Use positive logic for boolean expression */
#define negative 0	/* Use negative logic, i.e. 0 = true */

void epilogue(), prologue();
void leader(), header(), trailer();

void chan_init();
