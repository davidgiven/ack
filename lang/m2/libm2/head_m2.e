#
;
; (c) copyright 1988 by the Vrije Universiteit, Amsterdam, The Netherlands.
; See the copyright notice in the ACK home directory, in the file "Copyright".
;
;
; Module:	Modula-2 runtime startoff
; Author:	Ceriel J.H. Jacobs
; Version:	$Header$
;

 mes 2,EM_WSIZE,EM_PSIZE

#define STACKSIZE	2048	/* maximum stack size for a coroutine */

 exa handler
 exa environ
 exa argv
 exa argc
 exa CurrentProcess
 exa MainProcess
 exa StackBase
 exa MainLB
 exa StackSize
 exp $catch
 exp $init
 inp $trap_handler

mainroutine
 bss 2*EM_PSIZE,0,0

 exp $m_a_i_n
 pro $m_a_i_n, STACKSIZE

 loc STACKSIZE
 ste StackSize

 lor 0
 lae MainLB
 sti EM_PSIZE

 lal -EM_WSIZE
 adp EM_WSIZE
 lae StackBase
 sti EM_PSIZE

 lae mainroutine
 adp 2*EM_PSIZE
 dup EM_PSIZE
 lae CurrentProcess
 sti EM_PSIZE
 lae MainProcess
 sti EM_PSIZE

 lal EM_WSIZE+EM_PSIZE
 loi EM_PSIZE
 lae environ		; save environment pointer
 sti EM_PSIZE

 lal EM_WSIZE
 loi EM_PSIZE
 lae argv		; save argument pointer
 sti EM_PSIZE

 lol 0
 ste argc		; save argument count

 lpi $trap_handler
 sig
 asp EM_PSIZE
 cal $init
 cal $__M2M_
 cal $halt
 loc 0			; should not get here
 ret EM_WSIZE
 end

 pro $trap_handler,0
 lpi $trap_handler
 sig
 lol 0	; trap number
 lae handler
 loi EM_PSIZE
 lpi $catch
 lae handler
 sti EM_PSIZE
 cai
 asp EM_PSIZE+EM_WSIZE
 rtt
 end 0

