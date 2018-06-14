#
 mes 2,EM_WSIZE,EM_PSIZE
 exp $_close
 pro $_close,0
 lol 0
 loc 6
 mon
 zne *1
 loc 0
 ret EM_WSIZE
1
 ste errno
 loc -1
 ret EM_WSIZE
 end
