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

 exa handler
 exa environ
 exa argv
 exa argc
 exa MainLB
 exa bkillbss
 exp $catch
 exp $init
 inp $trap_handler

bkillbss
 bss EM_PSIZE,0,0

 exp $_m_a_i_n
 pro $_m_a_i_n, 0

 lor 0
 lae MainLB
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

