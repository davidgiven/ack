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

 exa _handler
 exa _environ
 exa _argv
 exa _argc
 exa _CurrentProcess
 exa _MainProcess
 exa _StackBase
 exa _MainLB
 exa _StackSize
 exp $_catch
 inp $trap_handler

_handler
 con $_catch
_environ
 bss EM_PSIZE,0,0
_argv
 bss EM_PSIZE,0,0
_argc
 bss EM_WSIZE,0,0
_CurrentProcess
 bss EM_PSIZE,0,0
_MainProcess
 bss EM_PSIZE,0,0
_StackBase
 bss EM_PSIZE,0,0
_MainLB
 bss EM_PSIZE,0,0
_StackSize
 bss EM_WSIZE,0,0
mainroutine
 bss 2*EM_PSIZE,0,0

 exp $m_a_i_n
 pro $m_a_i_n, STACKSIZE

 loc STACKSIZE
 ste _StackSize

 lor 0
 lae _MainLB
 sti EM_PSIZE

 lal -EM_WSIZE
 adp EM_WSIZE
 lae _StackBase
 sti EM_PSIZE

 lae mainroutine
 adp 2*EM_PSIZE
 dup EM_PSIZE
 lae _CurrentProcess
 sti EM_PSIZE
 lae _MainProcess
 sti EM_PSIZE

 lal EM_WSIZE+EM_PSIZE
 loi EM_PSIZE
 lae _environ		; save environment pointer
 sti EM_PSIZE

 lal EM_WSIZE
 loi EM_PSIZE
 lae _argv		; save argument pointer
 sti EM_PSIZE

 lol 0
 ste _argc		; save argument count

 lpi $trap_handler
 sig
 asp EM_PSIZE
 cal $_M2M
 cal $_halt
 loc 0			; should not get here
 ret EM_WSIZE
 end

 pro $trap_handler,0
 lol 0	; trap number
 lae _handler
 loi EM_PSIZE
 cai
 lpi $trap_handler
 sig
 asp EM_PSIZE+EM_WSIZE
 rtt
 end 0

