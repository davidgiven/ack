 mes 2,4,4

oldtrp
 bss 4, 0, 0

 exp $init
 pro $init, 0
 loc -321-1
 sim
 lpi $catch1
 sig
 ste oldtrp
 cal $initfile
 ret 0
 end 0

 pro $catch1, 0
 loe oldtrp
 sig
 asp 4
 loe 0
 loe 4
 lol 0
 cal $catch
 asp 12
 lol 0
 trp
 rtt
 end 0

 exp $now
 pro $now, 12
 zre deadlock
 lal -12
 loc 35
 mon
 asp 4
 lol -12
 ret 4
 end 12

 exp $block_mo
 pro $block_mo, 0
 lol 4
 lol 8
 lol 0
 bls 4
 ret 0
 end 0
