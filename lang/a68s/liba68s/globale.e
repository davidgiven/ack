#include "e.h"
 exa .HTOP		; the label holtop
 exa .1			; the Pascal global area
 exa _extfl		; the routine '_ini' puts 'input' & 'output' here
 exp $ESTART0
 exp $ESTART_
 exp $ESTOP_
 exp $ABORT


 ; PROCEDURE ESTART0
 pro $ESTART0,0
 lor 0			; my LB
 dup SZADDR
 dch			; _m_a_i_n's LB
 dup SZADDR
 str 0			; pretend I am in _m_a_i_n
 lae .HTOP-FIRSTIBOFFSET; destination address (holtop-firstiboffset)
 ; now calc how much to move
 lal 0
 lor 0
 sbs SZWORD		; subtract address of param from lb to get link space
 loc SZWORD+SZADDR+SZADDR
 ads SZWORD		; allow for one parameter of _m_a_i_n
 bls SZWORD		; block move
			; now the global area contains an exact copy of
			; _m_a_i_n's stack frame, and main will subsequently
			; adjust its LB to point to this global copy, thus
			; making it a part of the official stack.
 str 0			; get my LB back
 ret 0
 end 0

 ; PROCEDURE ESTART_ (INPUT,OUTPUT);
 pro $ESTART_,0
.2 
 con 2,0,0		; array that is to be _extfl
.3
 con 0I SZADDR		; PASCAL trap routine
.4
 con 0			; trapn
 con 0			; signaln
 LFL SZADDR+SZADDR	; base address for input (2nd param)
 lae .1
 sbs SZWORD		; subtract address from hol1 to get offset
 ste .2+SZWORD		; store in array of offsets
 LFL SZADDR		; and again for output (1st param after static link)
 lae .1
 sbs SZWORD
 ste .2+SZWORD+SZWORD	; store in array
 lxl 2			; params for _ini
 lae .2
 lae .1
 lxa 2
 cal $_ini
 asp SZADDR+SZADDR+SZADDR+SZADDR
 loc A68STAMP		; _m_a_i_n's frame stamp, for isa68, any positive number
 ste .HTOP-FSTAMPOFFSET ; it is in a SZWORD integer, 1st local var
 inp $_usigs
 cal $_usigs		; catch UNIX interrupts as EM trap 15
 inp $_acatch
 lpi $_acatch		; A68 trap routine
 sig
 lae .3
 sti SZWORD		; preserve PASCAL trap routine
 zre .4			; trapn
 ret 0
 end 0

	; procedure usigs;
	;   var i: integer;
	;     begin
	;     for i := 1 to 16 do signal(i, ucatch);
	;     end;
 pro $_usigs,SZWORD
 mes 9,0
 loc 1
 loc 16
 bgt *2
 loc 1
 stl -SZWORD
1
 zer SZWORD
 inp $_ucatch
 lpi $_ucatch
 lol -SZWORD
 cal $signal
 asp SZWORD+SZWORD+SZWORD
 lol -SZWORD
 loc 16
 beq *2
 lol -SZWORD
 inc 
 stl -SZWORD
 bra *1
2
 mes 3,-SZWORD,4,1
 ret 0
 end SZWORD

	; procedure ucatch(signo: integer);
	;     begin
	;     trap(15);
	;     end;
 pro $_ucatch,0
 mes 9,4
 lol 0
 ste .4+SZWORD		; signaln
#ifdef BSD4
 loc 0
 cal $sigsetmask	; unblock all signals
 asp SZWORD
 LLC 0			; SIG_DFL
 lol 0
 cal $signal		; because 4.2 Inices do not reset caught signals
 asp SZADDR+SZWORD
#endif
 loc 15
 cal $trap
 asp SZWORD
 mes 3,0,4,0
 ret 0
 end 0

 pro $_acatch,SZWORD
 loc PASCALSTAMP
 stl -SZWORD
 lol 0			; EM trap number
 dup SZWORD
 ste .4			; trapn
 ngi SZWORD
 lxl 0
 cal $ERRORR		; should never return
 end SZWORD

 pro $ESTOP_,0
 loc 0
 cal $_hlt
 end 0

 pro $ABORT,0
 loe .4			; trapn
 zne *1
 loc 1			; if abort is called then presumably some error has
			; occured, thus exit code 1
 cal $_hlt
1
 loe .4			; trapn
 loc 15
 bne *2			; if not a UNIX signal
 cal $_cleanup
 loe .4+SZWORD		; signaln
 cal $getpid
 lfr SZWORD
 cal $kill
2
 lae .3			; PASCAL trap routine
 loi SZWORD
 dup SZWORD
 zeq *3			; no PASCAL trap routine
 sig
 asp SZWORD
 loe .4
 trp			; now let PASCAL handle the same trap
3
 loe .4			; trapn
 cal $_catch
 end 0

