#
;
; (c) copyright 1988 by the Vrije Universiteit, Amsterdam, The Netherlands.
; See the copyright notice in the ACK home directory, in the file "Copyright".
;
;
; Module:	Interface to some EM instructions and data
; Author:	Ceriel J.H. Jacobs
; Version:	$Id$
;
 mes 2,EM_WSIZE,EM_PSIZE

#define ARG1    0
#define ARG2    EM_DSIZE
#define IRES    2*EM_DSIZE

; FIF is called with three parameters:
;       - address of integer part result (IRES)
;       - float two (ARG2)
;       - float one (ARG1)
; and returns an EM_DSIZE-byte floating point number
; Definition:
;	PROCEDURE FIF(ARG1, ARG2: LONGREAL; VAR IRES: LONGREAL) : LONGREAL;

 exp $FIF
 pro $FIF,0
 lal 0
 loi 2*EM_DSIZE
 fif EM_DSIZE
 lal IRES
 loi EM_PSIZE
 sti EM_DSIZE
 ret EM_DSIZE
 end ?

#define FARG    0
#define ERES    EM_DSIZE

; FEF is called with two parameters:
;       - address of base 2 exponent result (ERES)
;       - floating point number to be split (FARG)
; and returns an EM_DSIZE-byte floating point number (the mantissa)
; Definition:
;	PROCEDURE FEF(FARG: LONGREAL; VAR ERES: integer): LONGREAL;

 exp $FEF
 pro $FEF,0
 lal FARG
 loi EM_DSIZE
 fef EM_DSIZE
 lal ERES
 loi EM_PSIZE
 sti EM_WSIZE
 ret EM_DSIZE
 end ?

#define TRAP    0

; TRP is called with one parameter:
;       - trap number (TRAP)
; Definition:
; PROCEDURE TRP(trapno: INTEGER);

 exp $TRP
 pro $TRP, 0
 lol TRAP
 trp
 ret 0
 end ?

#define PROC    0

; SIG is called with one parameter:
;       - procedure instance identifier (PROC)
; and returns the old traphandler.

 exa handler
 exp $SIG
 pro $SIG, 0
 lae handler
 loi EM_PSIZE
 lal PROC
 loi EM_PSIZE
 lae handler
 sti EM_PSIZE
 ret EM_PSIZE
 end ?

 exp $LINO
 pro $LINO,0
 loe 0
 ret EM_WSIZE
 end ?

 exp $FILN
 pro $FILN,0
 lae 4
 loi EM_PSIZE
 ret EM_PSIZE
 end ?
