#define WS EM_WSIZE
#define PS EM_PSIZE
#include "test.h"
 mes 2,WS,PS
 mes 1
 mes 4,300
.000
 con "tst000"
 exp $m_a_i_n
 pro $m_a_i_n,0
 loc 123
 loc -98
; TEST 000: empty
 fil .000
 loc -98
 bne *1
 loc 123
 bne *1
 lin 0
 nop
 loc 0
 ret WS
1
 lin 1
 nop
 loc 1
 ret WS
 end
