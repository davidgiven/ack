#include "e.h"

 exp $PROCENTR		; calls through to the (lower case) pascal RT system
 exp $PROCEXIT

 ; these are calls through to the (lower case) pascal run-time system

 pro $PROCENTR,0
 LFL 0
 cal  $procentry
 asp  SZADDR
 ret  0
 end  0

 pro $PROCEXIT,0
 LFL 0
 cal  $procexit
 asp  SZADDR
 ret  0
 end  0
