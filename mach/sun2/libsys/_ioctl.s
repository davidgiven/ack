#include "syscall.h"
.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define __ioctl
__ioctl: SYSTEM(SYS_ioctl)
