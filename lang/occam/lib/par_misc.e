#
 mes 2,EM_WSIZE,EM_PSIZE
 exp $top_size
 pro $top_size, 3*EM_WSIZE+3*EM_PSIZE
 mes 11
 lal 0
 loi EM_PSIZE		; s_brk
 lor 1			; s_brk  SP
 sbs EM_PSIZE		; s_brk-SP
 ret EM_PSIZE		; return size of block to be saved
 end 3*EM_WSIZE+3*EM_PSIZE

 exp $top_save
 pro $top_save, 0
 loe 0
 lae 4			; load line number and file name
 loi EM_PSIZE
 lim			; ignore mask
 lor 0			; LB
 lal 0
 loi EM_PSIZE		; size of block
 loc EM_PSIZE
 loc EM_WSIZE
 cuu
 dup EM_WSIZE
 stl 0			; push & store size in 2 bytes
 lor 1			; SP (the SP BEFORE pushing)
 lor 1			; SP (address of stack top to save)
 lal EM_PSIZE			; area
 loi EM_PSIZE
 lol 0			; size
 bls EM_WSIZE		; move whole block
 asp 3*EM_PSIZE+3*EM_WSIZE	; remove the lot from the stack
 loc 1
 ret EM_WSIZE			; return 1
 end 0

 exp $top_load
 pro $top_load, 0
 lal 0
 loi EM_PSIZE
 dup EM_PSIZE
 loi EM_PSIZE
 sti EM_PSIZE		; copy pointer from argument 0 to new frame
 loi EM_PSIZE		; load indirect to
 str 1			; restore SP
 dup EM_PSIZE		; givesargument 0 again
 lor 1			; SP (the SP AFTER, see above)
 lor 1
 adp EM_PSIZE
 loi EM_PSIZE
 lof EM_PSIZE		; size of block
 bls EM_WSIZE		; move block back (SP becomes the SP BEFORE again!)
 asp EM_WSIZE		; drop size
 str 0			; LB
 sim			; ignore mask
 lae EM_PSIZE
 sti EM_PSIZE
 ste 0			; line and file
 loc 0
 ret EM_WSIZE		; return 0
 end 0
