/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/*  I N L I N E   S U B S T I T U T I O N
 *
 *  I L 3 _ A U X . H
 */

extern line_p last_line();	/* (line_p list) 
				 * Find the last line of a list.
				 */
extern app_list();		/* (line_p list,l)
				 * Put list after l
				 */
extern rem_line();		/* (line_p l)
				 * Remove a line from a (doubly linked)
				 * list.
				 */
