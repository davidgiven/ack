#
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
