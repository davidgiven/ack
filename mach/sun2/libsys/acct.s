#include "syscall.h"
.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define _acct
_acct: SYSTEM(SYS_acct)
