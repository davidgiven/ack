/* $Header$ */
/*	B L O C K   S T O R I N G   A N D   L O A D I N G	*/

#include	<em.h>
#include	"arith.h"
#include	"sizes.h"
#include	"atw.h"

/*	Because EM does not support the loading and storing of
	objects having other sizes than word fragment and multiple,
	we need to have a way of transferring these objects, whereby
	we simulate "loi" and "sti": the address of the source resp.
	destination is located on top of stack and a call is done
	to load_block() resp. store_block().
	===============================================================
	# Loadblock() works on the stack as follows: ([ ] indicates the
	# position of the stackpointer)
	# lower address--->
	# 1)	| &object
	# 2)	| ... ATW(sz) bytes ... | sz | &stack_block | &object
	# 3)	| ... ATW(sz) bytes ...
	===============================================================
	Loadblock() pushes ATW(sz) bytes directly onto the stack!

	Store_block() works on the stack as follows:
	lower address--->
	1)	| ... ATW(sz) bytes ... | &object
	2)	| ... ATW(sz) bytes ... | &object | &stack_block | sz
	3)	<empty>

	If sz is a legal argument for "loi" or "sti", just one EM
	instruction is generated.
	In the other cases, the notion of alignment is taken into account:
	we only push an object of the size accepted by EM onto the stack,
	while we need a loop to store the stack block into a memory object.
*/
store_block(sz, al)
	arith sz;
	int al;
{
	/* Next condition contains Lots of Irritating Stupid Parentheses
	*/
	if (
		((sz == al) && (word_align % al == 0)) ||
		(
			(sz % word_size == 0 || word_size % sz == 0) &&
			(al % word_align == 0)
		)
	)
		C_sti(sz);
	else	{
		/*	address of destination lies on the stack	*/

		/*	push address of first byte of block on stack onto
			the stack by computing it from the current stack
			pointer position
		*/
		C_lor((arith)1);	/* push current sp		*/
		C_adp(pointer_size);	/* set & to 1st byte of block	*/
		C_loc(sz);		/* number of bytes to transfer	*/
		C_cal("__stb");		/* call transfer routine	*/
		C_asp(pointer_size + pointer_size + int_size + ATW(sz));
	}
}

load_block(sz, al)
	arith sz;
	int al;
{
	arith esz = ATW(sz);	/* effective size == actual # pushed bytes */

	if ((sz == al) && (word_align % al == 0))
		C_loi(sz);
	else
	if (al % word_align == 0)
		C_loi(esz);
	else {
		/* do not try to understand this...	*/
		C_asp(-(esz - pointer_size));	/* allocate stack block */
		C_lor((arith)1);	/* push & of stack block as dst	*/
		C_dup(pointer_size);		/* fetch source address	*/
		C_adp(esz - pointer_size);
		C_loi(pointer_size);
		C_loc(sz);			/* # bytes to copy	*/
		C_cal("__stb");			/* library copy routine	*/
		C_asp(int_size + pointer_size + pointer_size);
	}
}
