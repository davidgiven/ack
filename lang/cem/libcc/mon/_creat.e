#
 mes 2,EM_WSIZE,EM_PSIZE
 exp $_creat
 pro $_creat,0
 lal 0
 loi EM_WSIZE+EM_PSIZE
 loc 8
 mon
 zeq *1
 ste errno              ; since e==r0
 loc -1
1
 ret EM_WSIZE
 end
