#include "syscall.h"
.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define _chroot
_chroot: SYSTEM(SYS_chroot)
