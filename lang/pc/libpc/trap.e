#

; $Header$
 mes 2,EM_WSIZE,EM_PSIZE

#define	TRAP	0

; trap is called with one parameter:
;	- trap number (TRAP)

 exp $trap
 pro $trap,0
 lol TRAP
 trp
 ret 0
 end ?
