#

; $Id$

 mes 2, EM_WSIZE, EM_PSIZE

 exp $__Get_LB
 pro $__Get_LB,0
 lor 0
1
 lol 0
 zlt *2
 dup EM_PSIZE
 zer EM_PSIZE
 cmp
 zeq *2
 dch
 del 0
 bra *1
2
 ret EM_PSIZE
 end

 exp $__Get_Hol0
 pro $__Get_Hol0,0
 lae 0
 ret EM_PSIZE
 end

 exp $__Get_HP
 pro $__Get_HP,0
 lor 2
 ret EM_PSIZE
 end

 exp $__Set_HP
 pro $__Set_HP,0
 lal 0
 loi EM_PSIZE
 str 2
 ret 0
 end

 exp $__Cvt_LB_AB
 pro $__Cvt_LB_AB,0
 lal 0
 loi EM_PSIZE
 zer EM_PSIZE
 cmp
 zeq *2
 lal 0
 loi EM_PSIZE
 lpb
 ret EM_PSIZE
2
 zer EM_PSIZE
 ret EM_PSIZE
 end
