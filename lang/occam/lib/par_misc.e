#
 mes 2,EM_WSIZE,EM_PSIZE
 exp $top_size
#ifdef __sparc
 inp $top_size2
 pro $top_size, 0
 mes 11
 zer EM_PSIZE
 lal 0
 loi EM_PSIZE
 cal $top_size2
 asp 2*EM_PSIZE
 lfr EM_WSIZE
 ret EM_WSIZE
 end 0
 pro $top_size2, 3*EM_WSIZE+3*EM_PSIZE
#else
 pro $top_size, 3*EM_WSIZE+3*EM_PSIZE
#endif
 mes 11
 lal 0
 loi EM_PSIZE		; s_brk
 lor 1			; s_brk  SP
 sbs EM_PSIZE		; s_brk-SP
 ret EM_PSIZE		; return size of block to be saved
 end 3*EM_WSIZE+3*EM_PSIZE

 exp $top_save
#ifdef __sparc
 inp $top_save2
 pro $top_save,0
 mes 11
 lal 0
 loi 2*EM_PSIZE
 cal $top_save2
 asp 2*EM_PSIZE
 lfr EM_WSIZE
 ret EM_WSIZE
 end 0
 pro $top_save2,0
#else
 pro $top_save, 0
#endif
 mes 11
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

sv
 bss EM_PSIZE, 0, 0

 exp $top_load
#ifdef __sparc
 inp $top_load1
 pro $top_load,0
 lal 0
 loi EM_PSIZE
 cal $top_load1
 asp EM_PSIZE
 lfr EM_WSIZE
 ret EM_WSIZE
 end 0
 pro $top_load1, 0
#else
 pro $top_load, 0
#endif
 mes 11
 lal 0
 loi EM_PSIZE
 lae sv
 sti EM_PSIZE

 lxl 0
2
 dup EM_PSIZE
 adp -3*EM_PSIZE
 lal 0
 loi EM_PSIZE           ; compare target SP with current LB to see if we must
 loi EM_PSIZE
 cmp                    ; find another LB first
 zgt *1
 dch                    ; just follow dynamic chain to make sure we find
                        ; a legal one
 bra *2
1
 str 0

 lae sv
 loi EM_PSIZE
 loi EM_PSIZE
 str 1			; restore SP
 asp -EM_PSIZE
 lae sv
 loi EM_PSIZE
 lor 1			; SP (the SP AFTER, see above)
 adp EM_PSIZE
 lae sv
 loi EM_PSIZE
 lof EM_PSIZE		; size of block
 bls EM_WSIZE		; move block back (SP becomes the SP BEFORE again!)
 asp EM_WSIZE+EM_PSIZE	; drop size + SP
 str 0			; LB
 sim			; ignore mask
 lae 4
 sti EM_PSIZE
 ste 0			; line and file
 loc 0
 ret EM_WSIZE		; return 0
 end 0
