#include "e.h"

 exp $SWAP

 pro $SWAP,0		; used by twist is A68S1CE to swap two items
			; of any length on the stack. the lengths of 
			; the two items to be swapped are passed as two
			; SZWORD byte parameters

 ; first copy what was the top item on the stack before the call
 ; to the procedural working stack
 lal SZADDR+SZWORD+SZWORD ; address of top of stack before call
 lol SZADDR+SZWORD        ; 2nd param is the length of top item
 los SZWORD		  ; load top item to procedural stack

 ; now copy what was the second stack item to the top
 lal SZADDR+SZWORD+SZWORD ; address of top for source of copy
 dup SZADDR		  ; dup for destination
 dup SZADDR		  ; and dup to copy top item back
 lol SZADDR+SZWORD	  ; length of top item 
 ads SZWORD		  ; add length to address of top of stack to get
			  ; the address of the 2nd param. (source address)
 exg SZADDR		  ; exchange source and destination address
 lol SZADDR		  ; 1st param is the length of the 2nd stack item
 bls SZWORD		  ; copy 2nd item to the top of the pre-call stack

 ; now copy the what was the top item from the procedural stack to
 ; the second position of the stack before the call
 lol SZADDR		  ; length of 2nd stack item, now on the top
 ads SZWORD		  ; add length to address of the top of the stack to
			  ; get place for 2nd item, the old top item
 lol SZADDR+SZWORD	  ; length of old top, now to be copied to 2nd position
 sts SZWORD		  ; move the item that was top
 ret 0
 end 0
