/* $Header$ */

/*	Get a unique id for C_insertpart, etc.
*/

int
C_getid()
{
	static int id = 0;

	return ++id;
}
