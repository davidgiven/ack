/* $Header$ */
/* INPUT PRIMITIVES */

#define	LoadChar(dest)	((dest = *ipp++) || (dest = loadbuf()))
#define	PushBack()	(ipp--)
				
/*	EOF may be defined as -1 in most programs but the character -1 may
	be expanded to the int -1 which causes troubles at the indexing in
	the class or boolean arrays.
*/
#define	EOI	(0200)
			
extern char *ipp;
