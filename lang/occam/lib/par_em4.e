 mes 2,4,4
 exp $top_size
 pro $top_size, 20
 lol 0			; s_brk
 lor 1			; s_brk  SP
 sbs 4			; s_brk-SP
 ret 4			; return size of block to be saved
 end 20

 exp $top_save
 pro $top_save, 0
 lde 0			; load line number and file name
 lim			; ignore mask
 lor 0			; LB
 lol 0			; size of block
 lor 1			; SP (the SP BEFORE pushing)
 lor 1			; SP (address of stack top to save)
 lol 4			; area
 lol 0			; size
 bls 4			; move whole block
 asp 24			; remove the lot from the stack
 loc 1
 ret 4			; return 1
 end 0

 exp $top_load
 pro $top_load, 0
 lol 0
 dup 4
 ste area		; copy area pointer from argument 0
 loi 4			; load indirect to
 str 1			; restore sp
 loe area		; load area, note that the SP is now correct
 lor 1			; SP (the SP AFTER, see above)
 loe area
 lof 4			; size of block
 bls 4			; move block back (SP becomes the SP BEFORE again!)
 asp 4			; drop size
 str 0			; LB
 sim			; ignore mask
 sde 0			; line and file
 loc 0
 ret 4			; return 0
 end 0
area
 bss 4,0,0
