#
 mes 2,EM_WSIZE,EM_PSIZE
 exp $_kill
 pro $_kill,0
 ldl 0
 loc 37
 mon
 zne *1
 loc 0
 ret EM_WSIZE
1
 ste errno
 loc -1
 ret EM_WSIZE
 end
