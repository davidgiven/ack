#
;
; (c) copyright 1988 by the Vrije Universiteit, Amsterdam, The Netherlands.
; See the copyright notice in the ACK home directory, in the file "Copyright".
;

; $Id$

 mes 2,EM_WSIZE,EM_PSIZE

#define TRAP    0

; _fptrp is called with one parameter:
;       - trap number (TRAP)

 exp $_fptrp
 pro $_fptrp,0
 lol TRAP
 trp
 ret 0
 end ?
