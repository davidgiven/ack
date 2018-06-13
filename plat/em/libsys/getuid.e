#
 mes 2,EM_WSIZE,EM_PSIZE
 exp $getuid
 pro $getuid,0
 loc 24
 mon
 asp EM_WSIZE
 ret EM_WSIZE
 end
