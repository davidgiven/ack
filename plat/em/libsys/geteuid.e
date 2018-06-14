#
 mes 2,EM_WSIZE,EM_PSIZE
 exp $geteuid
 pro $geteuid,0
 loc 24
 mon
 ret EM_WSIZE
 end
