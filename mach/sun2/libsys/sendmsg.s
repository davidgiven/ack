#include "syscall.h"
.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define _sendmsg
_sendmsg: SYSTEM(SYS_sendmsg)
