/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Id$ */
/*	B L O C K   S T O R I N G   A N D   L O A D I N G	*/

#include	"parameters.h"
#ifndef	LINT

#include <em.h>
#include <em_reg.h>
#include "arith.h"
#include "sizes.h"
#include "atw.h"
#include "align.h"
#ifndef STB
#include "label.h"
#include "stack.h"
#include "Lpars.h"
#include "blocks.h"
#include "macro.h"
#include "util.h"

#define LocalPtrVar()	NewLocal(pointer_size, pointer_align, reg_pointer, REGISTER)
#define LocalIntVar()	NewLocal(int_size, int_align, reg_any, REGISTER)
#endif /* STB */

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

int suitable_sz(arith sz, int al)
{
	return	((int)sz % (int)word_size == 0 && al % word_align == 0) ||
		(
			word_size % sz == 0 &&
			(al >= (int)sz || al >= word_align)
		/* Lots of Irritating Stupid Parentheses */
		);
}

void store_block(arith sz, int al)
{
	if (suitable_sz(sz, al))
		C_sti(sz);
	else {
#ifndef STB
		arith src, dst;

		/* allocate two pointer temporaries */
		src = LocalPtrVar();
		dst = LocalPtrVar();

		/* load the addresses */
		StoreLocal(dst, pointer_size);
		C_lor((arith)1);	/* push current sp */
		StoreLocal(src, pointer_size);
		copy_loop(sz, src, dst);
		C_asp(ATW(sz));
		FreeLocal(dst);
		FreeLocal(src);
#else /* STB */
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
#endif /* STB */
	}
}

void load_block(arith sz, int al)
{

	if (suitable_sz(sz, al))
		C_loi(sz);
	else {
#ifndef STB
		arith src, dst;

		/* allocate two pointer temporaries */
		src = LocalPtrVar();
		dst = LocalPtrVar();

		StoreLocal(src, pointer_size);
		C_asp(-ATW(sz));	/* allocate stack block */
		C_lor((arith)1);	/* push & of stack block as dst	*/
		StoreLocal(dst, pointer_size);
		copy_loop(sz, src, dst);
		FreeLocal(dst);
		FreeLocal(src);
#else /* STB */
		arith esz = ATW(sz) - pointer_size;
		C_asp(-esz);		/* allocate stack block */
		C_lor((arith)1);	/* push & of stack block as dst	*/
		C_dup(pointer_size);		/* fetch source address	*/
		C_adp(esz);
		C_loi(pointer_size);
		C_loc(sz);			/* # bytes to copy	*/
		C_cal("__stb");			/* library copy routine	*/
		C_asp(int_size + pointer_size + pointer_size);
#endif /* STB */
	}
}

void copy_block(arith sz, int al)
{

	if (suitable_sz(sz, al))
		C_blm(sz);
	else {
#ifndef STB
		arith src, dst;

		/* allocate two pointer temporaries */
		src = LocalPtrVar();
		dst = LocalPtrVar();

		StoreLocal(dst, pointer_size);
		StoreLocal(src, pointer_size);
		copy_loop(sz, src, dst);
		FreeLocal(dst);
		FreeLocal(src);
#else /* STB */
		C_loc(sz);			/* # bytes to copy	*/
		C_cal("__stb");			/* library copy routine	*/
		C_asp(int_size + pointer_size + pointer_size);
#endif /* STB */
	}
}

#ifndef STB
void copy_loop(arith sz, arith src, arith dst)
{
	/* generate inline byte-copy loop */
	label l_cont = text_label(), l_stop = text_label();
	arith tmp_sz = LocalIntVar();

	C_loc(sz);		/* amount of bytes */
	StoreLocal(tmp_sz, int_size);
	C_df_ilb(l_cont);
	LoadLocal(tmp_sz, int_size);
	C_zle(l_stop);
	C_del(tmp_sz);
	LoadLocal(src, pointer_size);
	C_dup(pointer_size);
	C_adp((arith)1);
	StoreLocal(src, pointer_size);
	C_loi((arith)1);
	LoadLocal(dst, pointer_size);
	C_dup(pointer_size);
	C_adp((arith)1);
	StoreLocal(dst, pointer_size);
	C_sti((arith)1);
	C_bra(l_cont);
	C_df_ilb(l_stop);
	FreeLocal(tmp_sz);
}
#endif /* STB */

#endif	/* LINT */

