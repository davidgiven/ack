/* $Header$ */
/* 	called by /lib/crt0.o; needed to suppress the loading of the
	standard exit() which performs unnecessary cleanup actions
*/

exit(n)
{
	_exit(n);
}
