#include "syscall.h"
.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define _ioctl
_ioctl: SYSTEM(SYS_ioctl)
