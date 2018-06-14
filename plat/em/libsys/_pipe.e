#
 mes 2,EM_WSIZE,EM_PSIZE
 exp $_pipe
 pro $_pipe,0
 loc 42
 mon
 zeq *1
 ste errno
 loc -1
 ret EM_WSIZE
1
 lal 0
 loi EM_PSIZE
 stf EM_WSIZE
 sil 0
 loc 0
 ret EM_WSIZE
 end
