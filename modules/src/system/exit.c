/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Header$ */
/* 	called by /lib/crt0.o; needed to suppress the loading of the
	standard exit() which performs unnecessary cleanup actions
*/

exit(n)
{
	_exit(n);
}
