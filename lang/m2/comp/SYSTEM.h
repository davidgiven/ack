/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 * Author: Ceriel J.H. Jacobs
 */

/* S Y S T E M   M O D U L E   T E X T */

/* $Header$ */

/* Text of SYSTEM module, for as far as it can be expressed in Modula-2 */

#define SYSTEMTEXT "DEFINITION MODULE SYSTEM;\n\
TYPE	PROCESS = ADDRESS;\n\
PROCEDURE NEWPROCESS(P:PROC; A:ADDRESS; n:CARDINAL; VAR p1:ADDRESS);\n\
PROCEDURE TRANSFER(VAR p1,p2:ADDRESS);\n\
END SYSTEM.\n"
