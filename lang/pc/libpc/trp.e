#

; $Header$
 mes 2,EM_WSIZE,EM_PSIZE

#define TRAP    0

; _trp() and trap() perform the same function,
; but have to be separate. trap exists to facilitate the user.
; _trp is there for the system, trap cannot be used for that purpose
; because a user might define its own Pascal routine called trap.

; _trp is called with one parameter:
;       - trap number (TRAP)

 exp $_trp
 pro $_trp,0
 lol TRAP
 trp
 ret 0
 end ?
