/*
  (c) copyright 1988 by the Vrije Universiteit, Amsterdam, The Netherlands.
  See the copyright notice in the ACK home directory, in the file "Copyright".
*/

/*
  Module:	get value on stack, byte by byte
  Author:	Ceriel J.H. Jacobs
  Version:	$Header$
*/
_load(siz, addr, p)
	register char *addr;
	register int siz;
{
	register char *q = (char *) &p;

	while (siz--) *q++ = *addr++;
}
