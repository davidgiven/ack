#
 mes 2,EM_WSIZE,EM_PSIZE
 exp $ptrace
 pro $ptrace,0
 zre errno
 lal 0
 loi EM_PSIZE+3*EM_WSIZE
 loc 26
 mon
 zeq *1
 ste errno
 loc -1
1
 ret EM_WSIZE
 end
