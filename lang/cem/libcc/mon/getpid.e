#
 mes 2,EM_WSIZE,EM_PSIZE
 exp $getpid
 pro $getpid,0
 loc 20
 mon
 ret EM_WSIZE
 end
