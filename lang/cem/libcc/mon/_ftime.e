#
 mes 2,EM_WSIZE,EM_PSIZE
 exp $_ftime
 pro $_ftime,0
 lal 0
 loi EM_PSIZE
 loc 35
 mon
 zne *1
 loc 0
 bra *2
1
 ste errno
 loc -1
2
 ret EM_WSIZE
 end
