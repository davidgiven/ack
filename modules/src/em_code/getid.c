/* $Header$ */

/*	Get a unique id for C_insertpart, etc.
*/

C_getid()
{
	static int id = 0;

	return ++id;
}
