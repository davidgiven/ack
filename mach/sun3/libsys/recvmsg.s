#include "syscall.h"
.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define _recvmsg
_recvmsg: SYSTEM(SYS_recvmsg)
