 mes 2,2,4
 exp $top_size
 pro $top_size, 14
 ldl 0			; s_brk
 lor 1			; s_brk  SP
 sbs 4			; s_brk-SP
 ret 4			; return size of block to be saved
 end 14

 exp $top_save
 pro $top_save, 0
 loe 0
 lde 4			; load line number and file name
 lim			; ignore mask
 lor 0			; LB
 ldl 0			; size of block
 loc 4
 loc 2
 cuu
 dup 2
 stl 0			; push & store size in 2 bytes
 lor 1			; SP (the SP BEFORE pushing)
 lor 1			; SP (address of stack top to save)
 ldl 4			; area
 lol 0			; size
 bls 2			; move whole block
 asp 18			; remove the lot from the stack
 loc 1
 ret 2			; return 1
 end 0

 exp $top_load
 pro $top_load, 0
 ldl 0
 dup 4
 sde area		; copy area pointer from argument 0
 loi 4			; load indirect to
 str 1			; restore SP
 lde area		; load area, note that the SP is now correct
 lor 1			; SP (the SP AFTER, see above)
 lde area
 lof 4			; size of block
 bls 2			; move block back (SP becomes the SP BEFORE again!)
 asp 2			; drop size
 str 0			; LB
 sim			; ignore mask
 sde 4
 ste 0			; line and file
 loc 0
 ret 2			; return 0
 end 0
area
 bss 4,0,0
