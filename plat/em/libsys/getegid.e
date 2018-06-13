#
 mes 2,EM_WSIZE,EM_PSIZE
 exp $getegid
 pro $getegid,0
 loc 47
 mon
 ret EM_WSIZE
 end
