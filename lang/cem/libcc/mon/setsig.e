#
 mes 2,EM_WSIZE,EM_PSIZE
save
        bss EM_PSIZE,0,0
trf
        bss EM_PSIZE,0,0

 exp $_setsig
 pro $_setsig,0
 lal 0
 loi EM_PSIZE
 lae trf
 sti EM_PSIZE
 inp $catchit
 lpi $catchit           ; trap to catchit
 sig
 lae save               ; remember higher level trap
 sti EM_PSIZE
 ret 0
 end

 pro $catchit,0
 lpi $catchit
 sig
 asp EM_PSIZE
 lol 0                  ; fetch trapno
 lae trf
 loi EM_PSIZE           ; fetch procedure identifier
 cai
 lfr EM_WSIZE
 zne *1
 ; trap not handled by trf function, trap through; trapno still on stack
 lae save
 loi EM_PSIZE
 sig
 asp EM_PSIZE
 trp
 lpi $catchit           ; trap ignored, further traps to catchit
 sig
 asp EM_PSIZE
 lae save               ; remember higher level trap
 sti EM_PSIZE
1
 rtt
 end
