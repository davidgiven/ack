#
;
; (c) copyright 1988 by the Vrije Universiteit, Amsterdam, The Netherlands.
; See the copyright notice in the ACK home directory, in the file "Copyright".
;
;
; Module:	implementation of coroutines
; Author:	Ceriel J.H. Jacobs
; Version:	$Header$
;
; This version works for EM implementations that have a contiguous downwards
; growing stack, on which data below the stack-pointer is not destroyed.
;
#include <em_mes.h>
#include <m2_traps.h>

 mes 2, EM_WSIZE, EM_PSIZE

 ; This file contains the implementation of the following routines from
 ; the SYSTEM module:
 ;	TRANSFER, NEWPROCESS
 ; The NEWPROCESS routine creates a new coroutine stack frame.
 ; The TRANSFER routine implements transfers from one coroutine to another.
 ; The memory organization for coroutines is rather complicated.
 ; One problem is caused by the fact that the user must allocate the
 ; stackspace. So, this stackspace can be located anywhere, including on
 ; the heap. This means that we cannot use this space as a stack, because
 ; in EM, the stack-pointer may never point below the heap-pointer.
 ; So, this space is only used to save the stack when the coroutine isn't
 ; running.
 ; It also contains information about the size of the frame, the
 ; address of the procedure that forms the coroutine body, the offset
 ; of the LB from the start of the frame, and the offset of the SP from
 ; the start of the frame.
 ; So, is looks like this:
 ;			|-----------------------------|
 ;                      |                             |
 ;                      |                             |
 ;                      |                             |
 ;					.
 ;					.
 ;					.
 ;                      |                             |
 ;                      |                             |
 ;                      |                             |	<--- coroutine ident
 ;			|-----------------------------|
 ;			|    saved SP                 |
 ;			|-----------------------------|
 ;			|    saved LB		      |
 ;			|-----------------------------|
 ;			|    procedure address or 0   |
 ;			|-----------------------------|
 ;			|	size		      |
 ;			|-----------------------------|
 ;
 ; Another problem is that the coroutines must always run at the same
 ; place in the stack. Therefore, in the runtime startoff a piece of the
 ; stack is allocated for coroutines.

 exp $_SYSTEM__NEWPROCESS
 exp $_SYSTEM__TRANSFER
 inp $ChkSize

 pro $_SYSTEM__NEWPROCESS, 0

 ; This procedure only initializes the area used for saving the stack.
 ; Its definition is:
 ;	PROCEDURE NEWPROCESS(P:PROC; A:ADDRESS; n:CARDINAL; VAR p1:ADDRESS);

 lol 2*EM_PSIZE		; size of frame (n)
 cal $ChkSize
 asp EM_WSIZE
 lfr EM_WSIZE
 sil EM_PSIZE		; store size in area (indicated by A)
 lal EM_PSIZE
 loi EM_PSIZE		; address of area (A)
 lal 0
 loi EM_PSIZE		; address of coroutine body (P)
 lal EM_PSIZE
 loi EM_PSIZE
 adp EM_WSIZE
 sti EM_PSIZE		; store it in area
 lal EM_PSIZE
 loi EM_PSIZE
 adp 3*EM_PSIZE + EM_WSIZE	; this becomes the coroutine identifier
 lal 2*EM_PSIZE+EM_WSIZE
 loi EM_PSIZE
 sti EM_PSIZE
 ret 0
 end 0

target
 bss EM_PSIZE, 0, 0

 pro $_SYSTEM__TRANSFER, 0

 ; This procedure does all the hard work.
 ; It must save the current environment, and restore the one to which the
 ; transfer is done. It must also make it look like the return is done
 ; from ITS invocation of transfer.
 ; Definition is:
 ;	PROCEDURE TRANSFER(VAR p1, p2 : ADDRESS);

 mes ms_gto	; This is a dangerous procedure

 lal EM_PSIZE
 loi EM_PSIZE
 loi EM_PSIZE	; address of target coroutine
 dup EM_PSIZE
 lae CurrentProcess
 loi EM_PSIZE
 dup EM_PSIZE
 lal 0
 loi EM_PSIZE	; address of place where to store address of current coroutine
 sti EM_PSIZE	; store
 cmp		; compare with current process
 zne *1
 ; Here, no real transfer needs to be done
 asp EM_PSIZE
 ret 0		; just return
1
 lae target
 sti EM_PSIZE	; store it in target

		; Now, we save the current stack
		; Use local base from main program

 lor 0		; load LB
 lae CurrentProcess
 loi EM_PSIZE
 adp -2*EM_PSIZE
 sti EM_PSIZE	; save it
 lor 1		; load SP
 lae CurrentProcess
 loi EM_PSIZE
 adp -EM_PSIZE
 sti EM_PSIZE	; save it
		; Now, we must find a stack we can temporarily use.
		; Just take the one from the main program.
 lae MainProcess
 loi EM_PSIZE
 adp -EM_PSIZE
 loi EM_PSIZE
 str 1		; temporary stackpointer

 lae MainLB
 loi EM_PSIZE
 str 0

 lae CurrentProcess
 loi EM_PSIZE
 lae MainProcess
 loi EM_PSIZE
 cmp
 zeq *2

 lae StackBase
 loi EM_PSIZE
 lae CurrentProcess
 loi EM_PSIZE
 adp -3*EM_PSIZE-EM_WSIZE
 loi EM_WSIZE	; get size
 ngi EM_WSIZE
 ads EM_WSIZE	; gives source address
 lae CurrentProcess
 loi EM_PSIZE	; destination address
 lae CurrentProcess
 loi EM_PSIZE
 adp -3*EM_PSIZE-EM_WSIZE
 loi EM_WSIZE
 bls EM_WSIZE	; copy

2
 lae target
 loi EM_PSIZE
 dup EM_PSIZE
 lae CurrentProcess
 sti EM_PSIZE	; store target process descriptor in _CurrentProcess
 lae MainProcess
 loi EM_PSIZE
 cmp
 zeq *4
		; Now check if the coroutine was called before
 lae target
 loi EM_PSIZE
 adp -3*EM_PSIZE
 loi EM_PSIZE
 zer EM_PSIZE
 cmp
 zeq *5
		; No, it was'nt
 lae StackBase
 loi EM_PSIZE
 str 1		; new stack pointer
 lae target
 loi EM_PSIZE
 adp -3*EM_PSIZE
 loi EM_PSIZE
 zer EM_PSIZE
 lae target
 loi EM_PSIZE
 adp -3*EM_PSIZE
 sti EM_PSIZE
 cai
 loc 0
 cal $exit
 ret 0
5
 lae target
 loi EM_PSIZE	; push source address
 lae StackBase
 loi EM_PSIZE	; subtract size from this and we have the destination address
 lae target
 loi EM_PSIZE
 adp -3*EM_PSIZE-EM_WSIZE
 loi EM_WSIZE
 ngi EM_WSIZE
 ads EM_WSIZE	; got it
 lae target
 loi EM_PSIZE
 adp -3*EM_PSIZE-EM_WSIZE
 loi EM_WSIZE
 bls EM_WSIZE
4
 lae target
 loi EM_PSIZE
 adp -2*EM_PSIZE
 loi EM_PSIZE
 str 0		; restore LB
 lae target
 loi EM_PSIZE
 adp -EM_PSIZE
 loi EM_PSIZE
 str 1		; restore SP
 ret 0
 end 0

 pro $ChkSize, 0
 lol 0
 loc 3*EM_PSIZE+EM_WSIZE
 sbi EM_WSIZE
 dup EM_WSIZE
 stl 0
 loe StackSize
 cmu EM_WSIZE
 zle *1
 loc M2_TOOLARGE	; trap number for "stack size too large"
 trp
1
 lol 0
 loc EM_WSIZE-1
 adi EM_WSIZE
 loc EM_WSIZE
 dvi EM_WSIZE
 loc EM_WSIZE
 mli EM_WSIZE
 ret EM_WSIZE
 end 0
