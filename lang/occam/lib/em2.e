 mes 2,2,4

oldtrp
 bss 4, 0, 0

 exp $init
 pro $init, 0
 loc -321-1
 sim
 lpi $catch1
 sig
 sde oldtrp
 cal $initfile
 ret 0
 end 0

 pro $catch1, 0
 lde oldtrp
 sig
 asp 4
 loe 0
 lde 4
 lol 0
 cal $catch
 asp 8
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
 asp 2
 ldl -12
 ret 4
 end 12

 exp $block_mo
 pro $block_mo, 0
 ldl 4
 ldl 8
 ldl 0
 loc 4
 loc 2
 cuu
 bls 2
 ret 0
 end 0
