#include "syscall.h"
.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define _quotactl
_quotactl: SYSTEM(SYS_quotactl)
