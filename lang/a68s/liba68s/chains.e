#include "e.h"
 exp $GETLINEN
 exp $ME
 exp $STATIC
 exp $DYNAMIC
 exp $ARGBASE
 exp $SETMYSTA
 exp $SETNSTAT
 exp $ISA68
 exp $ISPUT
 exp $ISGET
 exp $GETCALLE

 ; function getlineno :integer;
 pro $GETLINEN,0	; return line no from hol0
 loe 0
 ret SZWORD
 end 

 pro $ME,0
 lor 0			; lb -> stack
 dch			; caller's lb -> stack
 ret SZADDR   		; clb -> function result area
 end 0

 pro $STATIC,0
 LFL SZADDR 		; param (lb of caller) (after static link)
 lpb			; ab of param
 loi SZADDR		; static link of param
 ret SZADDR
 end 0

 pro $DYNAMIC,0
 LFL SZADDR		; param (lb of caller) (after static link)
 dch			; follow dynamic chain, using lb of caller
 ret SZADDR
 end 0

 pro $ARGBASE,0
 LFL SZADDR		; param (somebody's lb)
 lpb			; convert to somebody's ab
 ret SZADDR
 end 0

 pro $SETMYSTA,0
 LFL SZADDR		; place param on stack
 lor 0			; lb -> stack , for dch
 dch			; caller's lb -> stack
 lpb			; caller's ab
 sti SZADDR		; store param in caller's static link
 ret 0
 end 0

#ifndef VAX4
 pro $SETNSTAT,0	; called from RNSTART of insert n extra levels in
			; the static chain of A68 (i.e. RNSTART's caller);
			; there is guaranteed to be enough vacant space at
			; the top of the IB of A68
 lor 0			; my LB
 dch			; RNSTART LB
 adp SZADDR		; fictitious LB
			; BUT THIS MAY NOT BE LEGAL EM. REVIEW WHEN
			; RNSTART IS REWRITTEN.
 dup SZADDR
 lxa 2			; A68 AB
 loi SZADDR		; A68 static
 exg SZADDR
 lpb			; fictitious AB
 sti SZADDR		; (fictitious AB) := A68 static
 dup SZADDR
 lxa 2			; A68 AB
 sti SZADDR		; (A68 AB) := fictitious LB
2
 lol SZADDR		; n
 loc 1
 sbi SZWORD
 dup SZWORD
 stl SZADDR		; n := n-1
 zeq *3
 dup SZADDR
 lpb			; fictitious AB
 dup SZADDR
 loi SZADDR
 exg SZADDR
 SFF SZADDR		; (fictitious AB + 1) := (fictitious AB)
 adp SZADDR		; new fictitious LB
 dup SZADDR
 dup SZADDR
 lpb			; new fictitious AB
 SFF -SZADDR		; (new fictitious AB -l = old fictitious AB) :=
						; new fictitious LB
 bra *2			; with the new fictitious LB on the stack
3
 ret 0
 end 0
#else
 pro $SETNSTAT,SZWORD	; called from RNSTART of insert n extra levels in
			; the static chain of A68 (i.e. RNSTART's caller);
			; this version does not assume that the space between
			; LB and AB is a constant. It calls itself recursively
			; to create n activation records, whose static chains
			; are linked as required. The last activation then 
			; copies the return status block of RNSTART over itself,
			; so exiting from RNSTART but leaving the extra chains
			; still within the stack.
 lor 0			; SLB
 dch			; RLB
 dup SZADDR		; RLB | RLB
 dch			; RLB | ALB
 lpb			; RLB | AAB
 dup SZADDR		; RLB | AAB | AAB
 loi SZADDR		; RLB | AAB | (AAB)
 lor 0			; RLB | AAB | (AAB) | SLB
 dch			; RLB | AAB | (AAB) | RLB
 lpb			; RLB | AAB | (AAB) | RAB
 sti SZADDR		; RLB | AAB   (RAB) := (AAB)
 sti SZADDR		; (AAB) := RLB
			; now my caller (RNSTART the first time) has been linked
			; into the static chain of HIS caller.
 lol SZADDR		; n
 loc 1
 sbi SZWORD		; n-1
 dup SZWORD
 zeq *4
 lxl 1
 cal $SETNSTAT
 asp SZWORD+SZADDR	; but it should never return here
 nop
4			; now we must move the return status block of RNSTART
			; on top of our own. We are still statically within RNSTART
 LFL 0			; RLB
 dup SZADDR		; RLB | RLB
 lpb			; RLB | RAB
 exg SZADDR		; RAB | RLB
 sbs SZWORD		; Amount to be moved
 dup SZWORD		; A | A
 stl -SZWORD		; A
 lor 0			; A | SLB
 dup SZADDR		; A | SLB | SLB
 lpb			; A | SLB | SAB
 exg SZADDR		; A | SAB | SLB
 sbs SZWORD		; A | SA (the size of our own return status block)
 exg SZWORD		; SA | A
 sbi SZWORD		; SA-A
 lor 0			; SLB
 ads SZWORD		; new SLB to be
 str 1			; set SP there
 lor 1			; TO (=SP the destination of the move)
 dup SZADDR		; TO | TO
 LFL 0			; TO | TO | RLB (the source of the move)
 exg SZADDR		; TO | RLB | TO
 lol -SZWORD		; TO | RLB | TO | A
 bls SZWORD		; TO
 str 0			; set SLB to the moved copy of RNSTART's block
 ret 0			; return to RNSTART's caller, resetting his registers
 end SZWORD		; one local to store A
#endif

 pro $GETCALLE,0	; returns LB of nearest A68 frame on stack, A68 caller
 LFL SZADDR		; param (lb of callee, routine) (after static link)
1
 dch			; follow dynamic chain
 dup SZADDR		; duplicate either to return or follow next time
 lof -SZWORD		; lb - SZWORD is addres of frame stamp
 loc A68STAMP
 bne *1			; do again if not A68 frame
 ret SZADDR		; return lb of frame
 end 0

 pro $ISA68,0
 LFL SZADDR		; get param, lb of frame to test
 adp -SZWORD
 loi SZWORD		; load frame stamp
 loc A68STAMP
 cmi SZWORD		; compare it with 'a68stamp'
 teq			; is it the same
 ret SZWORD		; return answer, true=1 false=0
 end 0

 pro $ISPUT,0
 LFL SZADDR		; get param, lb of frame to test
 adp -SZWORD
 loi SZWORD		; load  frame stamp
 loc PUTSTAMP
 cmi SZWORD             ; compare it with 'putstamp'
 teq
 ret SZWORD		; return answer, true=1 false=0
 end 0

 pro $ISGET,0
 LFL SZADDR		; get param, lb of frame to test
 adp -SZWORD
 loi SZWORD		; load  frame stamp
 loc GETSTAMP
 cmi SZWORD             ; compare it with 'getstamp'
 teq
 ret SZWORD		; return answer, true=1 false=0
 end 0
