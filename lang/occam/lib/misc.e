#
 mes 2,EM_WSIZE,EM_PSIZE

oldtrp
 bss EM_PSIZE, 0, 0

 exp $init
 pro $init, 0
 loc -321-1
 sim
 lpi $catch1
 sig
 lae oldtrp
 sti EM_PSIZE
 cal $initfile
 ret 0
 end 0

 pro $catch1, 0
 lae oldtrp
 loi EM_PSIZE
 sig
 asp EM_PSIZE
 loe 0
 lae 4
 loi EM_PSIZE
 lol 0
 cal $catch
 asp 2*EM_WSIZE+EM_PSIZE
 lol 0
 trp
 rtt
 end 0

 exp $block_move
 pro $block_move, 0
 lal EM_PSIZE
 loi EM_PSIZE
 lal 2*EM_PSIZE
 loi EM_PSIZE

 lal 0
 loi EM_PSIZE
 loc EM_PSIZE
 loc EM_WSIZE
 cuu
 bls EM_WSIZE
 ret 0
 end 0
